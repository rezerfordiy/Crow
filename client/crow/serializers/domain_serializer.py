import json
from crow.domain.Point import Point
from crow.domain.Rectangle import Rectangle
from crow.domain.SceneConfig import SceneConfig

class DomainEncoder(json.JSONEncoder):
    def default(self, obj):
        if isinstance(obj, (Point, Rectangle, SceneConfig)):
            return obj.to_dict()
        
        if isinstance(obj, list) and obj and isinstance(obj[0], (Point, Rectangle, SceneConfig)):
            return [item.to_dict() for item in obj]
        
        return super().default(obj)