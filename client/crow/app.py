from flask import Flask

from crow.rest import sceneconfig
from crow.settings import DevConfig


def create_app(config_object=DevConfig):
    app = Flask(__name__)
    app.config.from_object(config_object)
    app.register_blueprint(sceneconfig.blueprint)
    return app
