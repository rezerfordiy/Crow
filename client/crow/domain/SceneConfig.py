from crow.shared.domain_model import DomainModel
from crow.domain.Point import Point
from crow.domain.Rectangle import Rectangle


class SceneConfig(object):
    def __init__(self, p1, p2, lstObstacles, lstSafeSones):
       self.start = p1
       self.end = p2
       self.obstacles = lstObstacles
       self.safesones = lstSafeSones

    @classmethod
    def from_dict(cls, adict):
        rect = SceneConfig(
        p1=Point.from_dict(adict['start']),
        p2=Point.from_dict(adict['end']),
        lstObstacles=[Rectangle.from_dict(i) for i in adict['obstacles']],
        lstSafeSones=[Rectangle.from_dict(i) for i in adict['safezones']]
        )

        return rect
    
    def to_dict(self):
        return {
            'start': self.start.to_dict(),
            'end': self.end.to_dict(),
            'obstacles': [i.to_dict() for i in self.obstacles],
            'safezones': [i.to_dict() for i in self.safesones]
        }




DomainModel.register(SceneConfig)    
