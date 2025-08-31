
#include "ProtocolHandler.h"
#include "GrpcProtocolHandler.h"
#include "HttpProtocolHandler.h"

std::unique_ptr<ProtocolHandler> ProtocolHandler::create(const QString& protocol,
                                                       SceneManager* manager,
                                                       const QString& address) {
    if (protocol == "grpc") {
        return std::make_unique<GrpcProtocolHandler>(manager, address);
    } else if (protocol == "http") {
        return std::make_unique<HttpProtocolHandler>(manager, address);
    }

    return nullptr;
}

QStringList ProtocolHandler::availableProtocols() {
    return {"grpc", "http"};
}
