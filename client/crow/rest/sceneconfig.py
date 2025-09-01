import json
from flask import Blueprint, request, Response

from crow.use_cases import request_objects as req
from crow.shared import response_object as res
from crow.repository import memrepo as mr
from crow.use_cases import storageroom_use_cases as uc
from crow.serializers import storageroom_serializer as ser

blueprint = Blueprint('storageroom', __name__)

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
    'zafezones': [

    ]
}
storageroom1 = {
    'code': 'f853578c-fc0f-4e65-81b8-566c5dffa35a',
    'size': 215,
    'price': 39,
    'longitude': -0.09998975,
    'latitude': 51.75436293,
}

storageroom2 = {
    'code': 'fe2c3195-aeff-487a-a08f-e0bdc0ec6e9a',
    'size': 405,
    'price': 66,
    'longitude': 0.18228006,
    'latitude': 51.74640997,
}

storageroom3 = {
    'code': '913694c6-435a-4366-ba0d-da5334a611b2',
    'size': 56,
    'price': 60,
    'longitude': 0.27891577,
    'latitude': 51.45994069,
}


@blueprint.route('/storagerooms', methods=['GET'])
def storageroom():
    qrystr_params = {
        'filters': {},
    }

    for arg, values in request.args.items():
        if arg.startswith('filter_'):
            qrystr_params['filters'][arg.replace('filter_', '')] = values

    request_object = req.SceneConfigGetRequestObject.from_dict()

    repo = mr.MemRepo(sceneconfig1)
    use_case = uc.SceneConfigGetUseCase(repo)

    response = use_case.execute(request_object)

    return Response(json.dumps(response.value, cls=ser.StorageRoomEncoder),
                    mimetype='application/json',
                    status=STATUS_CODES[response.type])
