
import grpc
import scene_pb2
import scene_pb2_grpc
import time

def test_scene_operations():
    channel = grpc.insecure_channel('localhost:50051')
    stub = scene_pb2_grpc.SceneServiceStub(channel)
    
    try:
        print("Clearing scene...")
        response = stub.ClearScene(scene_pb2.Empty())
        print(f"Clear: {response.message}")
        
        print("Updating scene...")
        config = scene_pb2.SceneConfig()
        
        config.start.x = 50
        config.start.y = 50
        config.end.x = 700
        config.end.y = 350
        
        obstacle1 = config.obstacles.add()
        obstacle1.topleft.x = 100
        obstacle1.topleft.y = 100
        obstacle1.bottomright.x = 140
        obstacle1.bottomright.y = 140
        
        obstacle2 = config.obstacles.add()
        obstacle2.topleft.x = 200
        obstacle2.topleft.y = 200
        obstacle2.bottomright.x = 240
        obstacle2.bottomright.y = 240
        
        zone = config.safezones.add()
        zone.topleft.x = 300
        zone.topleft.y = 300
        zone.bottomright.x = 400
        zone.bottomright.y = 400
        
        response = stub.UpdateScene(config)
        print(f"Update: {response.message}")
        
        print("Getting scene...")
        current_scene = stub.GetScene(scene_pb2.Empty())
        print(f"Scene has {len(current_scene.obstacles)} obstacles")
        print(f"Start: ({current_scene.start.x}, {current_scene.start.y})")
        print(f"End: ({current_scene.end.x}, {current_scene.end.y})")
        
    except grpc.RpcError as e:
        print(f"gRPC error: {e}")
    except Exception as e:
        print(f"Error: {e}")

if __name__ == '__main__':
    test_scene_operations()
