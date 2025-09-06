
from crow.shared import request_object as req

class SceneConfigGetRequestObject(req.ValidRequestObject):

    @classmethod
    def from_dict(cls):
        # invalid_req = InvalidRequestObject()

        # if 'filters' in adict and not isinstance(adict['filters'], collections.Mapping):
        #     invalid_req.add_error('filters', 'Is not iterable')

        # if invalid_req.has_errors():
        #     return invalid_req

        return SceneConfigGetRequestObject()

