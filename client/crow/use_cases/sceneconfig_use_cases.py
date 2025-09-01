from crow.shared import response_object as res        
from crow.shared import use_case as uc


class SceneConfigGetUseCase(uc.UseCase):

    def __init__(self, repo):
        self.repo = repo

    def process_request(self, request_object):
        domain_storageroom = self.repo.get()
        return res.ResponseSuccess(domain_storageroom)