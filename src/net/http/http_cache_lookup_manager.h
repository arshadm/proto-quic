// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NET_HTTP_HTTP_CACHE_LOOKUP_MANAGER_H_
#define NET_HTTP_HTTP_CACHE_LOOKUP_MANAGER_H_

#include "net/base/net_export.h"
#include "net/http/http_cache.h"
#include "net/http/http_cache_transaction.h"
#include "net/spdy/server_push_delegate.h"

namespace net {

// An implementation of ServerPushDelegate that issues an HttpCache::Transaction
// to lookup whether the response to the pushed URL is cached and cancel the
// push in that case.
class NET_EXPORT_PRIVATE HttpCacheLookupManager : public ServerPushDelegate {
 public:
  // |http_cache| MUST outlive the HttpCacheLookupManager.
  HttpCacheLookupManager(HttpCache* http_cache,
                         const NetLogWithSource& net_log);
  ~HttpCacheLookupManager() override;

  void OnPush(std::unique_ptr<ServerPushHelper> push_helper) override;

  // Invoked when the HttpCache::Transaction for |url| finishes to cancel the
  // server push if the response to the server push is found cached.
  void OnLookupComplete(const GURL& url, int rv);

 private:
  // A class that takes the ownership of ServerPushHelper, issues and owns an
  // HttpCache::Transaction which lookups the response in cache for the server
  // push.
  class LookupTransaction {
   public:
    LookupTransaction(std::unique_ptr<ServerPushHelper> push_helper);
    ~LookupTransaction();

    // Issues an HttpCache::Transaction to lookup whether the response is cached
    // without header validation.
    int StartLookup(HttpCache* cache,
                    const CompletionCallback& callback,
                    const NetLogWithSource& net_log);

    void CancelPush();

   private:
    std::unique_ptr<ServerPushHelper> push_helper_;
    std::unique_ptr<HttpRequestInfo> request_;
    std::unique_ptr<HttpTransaction> transaction_;
  };

  NetLogWithSource net_log_;
  // HttpCache must outlive the HttpCacheLookupManager.
  HttpCache* http_cache_;
  std::map<GURL, std::unique_ptr<LookupTransaction>> lookup_transactions_;
  base::WeakPtrFactory<HttpCacheLookupManager> weak_factory_;
};

}  // namespace net

#endif  // NET_HTTP_HTTP_CACHE_LOOKUP_MANAGER_H_
