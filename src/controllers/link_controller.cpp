#include <controllers/link_controller.h>

std::shared_ptr<cutr::service::LinkService> LinkController::linkService_ = nullptr;
std::shared_ptr<cutr::service::RedirectService> LinkController::redirectService_ = nullptr;

void LinkController::setServices(std::shared_ptr<cutr::service::LinkService> link,
                                 std::shared_ptr<cutr::service::RedirectService> redirect) {
    linkService_ = std::move(link);
    redirectService_ = std::move(redirect);
}


void LinkController::createShortLink(const drogon::HttpRequestPtr &req,
                                     std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
    auto handle = [req, callback = std::move(callback), this]() -> drogon::Task<void> {
        try {
            auto jsonReq = req->getJsonObject();
            if (!jsonReq || !(*jsonReq)["url"].isString()) {
                auto resp = drogon::HttpResponse::newHttpJsonResponse(
                        Json::Value{{"error", "Missing 'url' in request"}}
                );
                resp->setStatusCode(drogon::k400BadRequest);
                callback(resp);
                co_return;
            }

            std::string originalUrl = (*jsonReq)["url"].asString();
            std::string shortCode = co_await linkService_->createShortLink(originalUrl);

            Json::Value jsonResp;
            jsonResp["shortCode"] = shortCode;

            auto resp = drogon::HttpResponse::newHttpJsonResponse(jsonResp);
            callback(resp);
        } catch (const std::exception &e) {
            auto resp = drogon::HttpResponse::newHttpJsonResponse(
                    Json::Value{{"error", e.what()}}
            );
            resp->setStatusCode(drogon::k500InternalServerError);
            callback(resp);
        }
        co_return;
    };
    handle();
}

void LinkController::redirectToOriginal(const drogon::HttpRequestPtr &req,
                                        std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                                        const std::string &hash) {
    auto handle = [hash, callback = std::move(callback), this]() -> drogon::Task<void> {
        try {
            auto urlOpt = co_await redirectService_->getOriginalUrl(hash);

            if (!urlOpt.has_value()) {
                auto resp = drogon::HttpResponse::newHttpResponse();
                resp->setStatusCode(drogon::k404NotFound);
                resp->setBody("Short link not found");
                callback(resp);
                co_return;
            }

            auto resp = drogon::HttpResponse::newHttpResponse();
            resp->setStatusCode(drogon::k302Found);
            resp->addHeader("Location", *urlOpt);
            callback(resp);
        } catch (const std::exception &e) {
            auto resp = drogon::HttpResponse::newHttpResponse();
            resp->setStatusCode(drogon::k500InternalServerError);
            resp->setBody(e.what());
            callback(resp);
        }
        co_return;
    };
    handle();
}