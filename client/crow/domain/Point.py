from crow.shared.domain_model import DomainModel

class Point(object):
    def __init__(self, x, y):
       self.x = x
       self.y = y

    @classmethod
    def from_dict(cls, adict):
        pnt = Point(
        x=adict['x'],
        y=adict['y'],
        )

        return pnt
    
    def to_dict(self):
        return {
            'x': self.x,
            'y': self.y
        }

DomainModel.register(Point)