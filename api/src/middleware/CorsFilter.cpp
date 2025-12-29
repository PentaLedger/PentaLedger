#include "middleware/CorsFilter.h"
#include <drogon/HttpResponse.h>

void CorsFilter::doFilter(const drogon::HttpRequestPtr &req,
                        drogon::FilterCallback &&fcb,
                        drogon::FilterChainCallback &&fccb)
{
    // Handle preflight OPTIONS requests
    if (req->getMethod() == drogon::Options)
    {
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode(drogon::k200OK);
        resp->addHeader("Access-Control-Allow-Origin", "*");
        resp->addHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS, PATCH");
        resp->addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization, X-Requested-With");
        resp->addHeader("Access-Control-Max-Age", "3600");
        fcb(resp);
        return;
    }

    // For all other requests, continue the filter chain
    // CORS headers will be added by global post-handling advice in main.cpp
    fccb();
}

