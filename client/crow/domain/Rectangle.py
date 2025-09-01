from crow.shared.domain_model import DomainModel
from crow.domain.Point import Point


class Rectangle(object):
    def __init__(self, p1, p2):
       self.topleft = p1
       self.bottomright = p2

    @classmethod
    def from_dict(cls, adict):
        rect = Rectangle(
        p1=Point.from_dict(adict['topleft']),
        p2=Point.from_dict(adict['bottomright']),
        )

        return rect
    
    def to_dict(self):
        return {
            'topleft': self.topleft.to_dict(),
            'bottomright': self.bottomright.to_dict()
        }

DomainModel.register(Rectangle)    