import json
import requests
from crow.use_cases import request_object as req
from crow.shared import response_object as res
from crow.repository import memrepo as mr
from crow.use_cases import sceneconfig_use_cases as uc
from crow.serializers import domain_serializer as ser

sceneconfig1 = {
    'start': {'x': 100, 'y': 100},
    'end': {'x': 140, 'y': 140}, 
    'obstacles': [
        {
            'topleft': {'x': 200, 'y': 200},
            'bottomright': {'x': 240, 'y': 240}
        }
    ],
    'safezones': []
}

request_object = req.SceneConfigGetRequestObject.from_dict(sceneconfig1)
repo = mr.MemRepo(sceneconfig1)
use_case = uc.SceneConfigGetUseCase(repo)
response = use_case.execute(request_object)

TARGET_URL = "http://127.0.0.1:8080/scene"
try:
    json_data = json.dumps(response.value, cls=ser.DomainEncoder)
    print(json_data)
    post_response = requests.post(
        TARGET_URL,
        data=json_data,
        headers={'Content-Type': 'application/json'}
    )
    
    print(f"Status Code: {post_response.status_code}")
    print(f"Response: {post_response.text}")
    
except requests.exceptions.RequestException as e:
    print(f"Error sending POST request: {e}")
