import json
from flask import Blueprint, request, Response

from crow.use_cases import request_object as req
from crow.shared import response_object as res
from crow.repository import memrepo as mr
from crow.use_cases import sceneconfig_use_cases as uc
from crow.serializers import domain_serializer as ser


blueprint = Blueprint('sceneconfig', __name__)

STATUS_CODES = {
    res.ResponseSuccess.SUCCESS: 200,
    res.ResponseFailure.RESOURCE_ERROR: 404,
    res.ResponseFailure.PARAMETERS_ERROR: 400,
    res.ResponseFailure.SYSTEM_ERROR: 500
}

sceneconfig1 = {
    'start': {
        'x': 100,
        'y': 100
        },
    'end': {
        'x': 140,
        'y': 140
        }, 
    'obstacles': [
        {
            'topleft': {
                'x': 200,
                'y': 200
            },
            'bottomright': {
                'x': 240,
                'y': 240
            }
        },

    ],
    'safezones': [

    ]
}

@blueprint.route('/get_route', methods=['GET'])
def storageroom():
    request_object = req.SceneConfigGetRequestObject.from_dict()
    repo = mr.MemRepo(sceneconfig1)
    use_case = uc.SceneConfigGetUseCase(repo)
    response = use_case.execute(request_object)

    return Response(json.dumps(response.value, cls=ser.DomainEncoder),
                    mimetype='application/json',
                    status=STATUS_CODES[response.type])
