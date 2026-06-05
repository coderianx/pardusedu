from fastapi import FastAPI, HTTPException, Request
from fastapi.responses import JSONResponse, HTMLResponse
from fastapi.templating import Jinja2Templates
from models import Notes
import sqlite3
import uuid
import redis
from datetime import datetime, timedelta
from contextlib import contextmanager
import time
import string
import secrets


app = FastAPI()
templates = Jinja2Templates(directory="templates")

DB_FILE = "notes.db"

redis_client = redis.Redis(
    host="localhost",
    port=6379,
    decode_responses=True
)

def generate_id(n=16):
    chars = string.ascii_letters + string.digits
    return ''.join(secrets.choice(chars) for _ in range(n))


RATE_LIMIT = 10
TIME_WINDOW = 60

@contextmanager
def get_db():
    conn = sqlite3.connect(DB_FILE)
    conn.row_factory = sqlite3.Row
    try:
        yield conn
        conn.commit()
    finally:
        conn.close()



def check_rate_limit(ip: str):
    key = f"rate:{ip}"
    now = int(time.time())

    redis_client.lpush(key, now)
    redis_client.ltrim(key, 0, RATE_LIMIT - 1)

    timestamps = redis_client.lrange(key, 0, -1)

    valid = [
        int(t) for t in timestamps
        if now - int(t) <= TIME_WINDOW
    ]

    if len(valid) > RATE_LIMIT:
        raise HTTPException(
            status_code=429,
            detail="Çok sayıda istek gönderildi"
        )



def init_db():
    with get_db() as conn:
        cur = conn.cursor()

        cur.execute("""
            CREATE TABLE IF NOT EXISTS notes (
                id TEXT PRIMARY KEY,
                title TEXT NOT NULL DEFAULT '',
                content TEXT NOT NULL,
                expires_at TEXT NOT NULL
            )
        """)

        cur.execute("""
            CREATE INDEX IF NOT EXISTS idx_notes_expires
            ON notes(expires_at)
        """)

init_db()

@app.middleware("http")
async def rate_limit_middleware(request: Request, call_next):

    ip = request.client.host

    try:
        check_rate_limit(ip)
    except HTTPException as e:
        return JSONResponse(
            status_code=e.status_code,
            content={"error": e.detail}
        )

    return await call_next(request)


@app.get("/")
def root():
    return {"server": "ok"}


@app.post("/notes")
def create_note(note: Notes):

    if not note.title.strip():
        raise HTTPException(400, "Title empty")

    if len(note.title) > 100:
        raise HTTPException(400, "Title too long")

    if len(note.content) > 500_000:
        raise HTTPException(413, "Note too large")

    
    expires_at = (datetime.utcnow() + timedelta(days=7)).isoformat()
    note_id = generate_id(16)

    with get_db() as conn:
        cur = conn.cursor()

        cur.execute("""
            INSERT INTO notes (id, title, content, expires_at)
            VALUES (?, ?, ?, ?)
        """, (
            note_id,
            note.title,
            note.content,
            expires_at
        ))

    return {
        "success": True,
        "data": {
            "id": note_id,
            "title": note.title,
            "expires_at": expires_at,
            "link": f"http://192.168.1.214:8000/n/{note_id}",
            "web_viewer_link": f"http://192.168.1.214:8000/n/view/{note_id}"
        }
    }


@app.get("/n/view/{note_id}", response_class=HTMLResponse)
def view_note_html(note_id: str, request: Request):
    row = _fetch_note(note_id)
    if row is None:
        return templates.TemplateResponse(request, "404.html", status_code=404)

    return templates.TemplateResponse(request, "note.html", {
        "title": row["title"],
        "content": row["content"],
        "expires_at": row["expires_at"]
    })


@app.get("/n/{note_id}")
def get_note(note_id: str):
    row = _fetch_note(note_id)
    if row is None:
        raise HTTPException(404, "Note not found")
    return {
        "id": note_id,
        "title": row["title"],
        "content": row["content"],
        "expires_at": row["expires_at"]
    }


def _fetch_note(note_id: str):
    with get_db() as conn:
        cur = conn.cursor()
        cur.execute("""
            SELECT title, content, expires_at
            FROM notes
            WHERE id = ?
        """, (note_id,))
        row = cur.fetchone()
        if row is None:
            return None
        expires_at = datetime.fromisoformat(row["expires_at"])
        if datetime.utcnow() > expires_at:
            cur.execute("DELETE FROM notes WHERE id = ?", (note_id,))
            return None
        return row

if __name__ == "__main__":
    import uvicorn
    uvicorn.run("main:app", host="0.0.0.0", port=8000)