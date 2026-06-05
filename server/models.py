from pydantic import BaseModel

class Notes(BaseModel):
    title: str = ""
    content: str
