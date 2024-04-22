#include "hello.hpp"

#include <fmt/format.h>

#include <string>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/clients/dns/component.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include "userver/cache/base_postgres_cache.hpp"
#include "userver/components/component_context.hpp"
#include "userver/storages/mysql/cluster_host_type.hpp"
#include "userver/storages/mysql/query.hpp"
#include "userver/storages/secdist/component.hpp"
#include "userver/storages/secdist/provider_component.hpp"
#include "userver/storages/secdist/secdist.hpp"
#include "userver/storages/mysql/query.hpp"
#include <userver/storages/mysql/component.hpp>
#include <userver/storages/mysql/cluster.hpp>
#include <userver/storages/mysql.hpp>
#include "cache_mysql.hpp"

namespace service_template {

namespace {

using MY_CACHE_POSTGRE = userver::components::PostgreCache<PostgresTrivialPolicy>;
using MY_CACHE_MYSQL = userver::components::MySqlCache<MySQLTrivialPolicy>;

class Hello final : public userver::server::handlers::HttpHandlerBase {
public:
  static constexpr std::string_view kName = "handler-hello";

  Hello(const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& component_context)
      : HttpHandlerBase(config, component_context),
        pg_cluster_(
            component_context
                .FindComponent<userver::components::Postgres>("postgres-db")
                .GetCluster()
        ),
        mysql_cluster_(
          component_context.FindComponent<userver::storages::mysql::Component>("mysql-db").GetCluster()
        ),
        pg_cache_(
          &component_context.FindComponent<MY_CACHE_POSTGRE>("pg-cache")
        ),
        mysql_cache_(
          &component_context.FindComponent<MY_CACHE_MYSQL>("mysql-cache")
        ) 
        {}

  std::string HandleRequestThrow(
      const userver::server::http::HttpRequest& request,
      userver::server::request::RequestContext& ) const override {
    std::string res;
    auto elem = pg_cache_->Get();
    for (auto struct_env : *elem) {
      res += "\n";
      res += fmt::to_string(struct_env.second.id);
      res += ",";
      res += fmt::to_string(struct_env.second.name);
    }
    auto elem2 = mysql_cache_->Get();
    for (auto struct_env : *elem2) {
      res += "\n(mysql)";
      res += fmt::to_string(struct_env.second.id);
      res += ",";
      res += fmt::to_string(*struct_env.second.name);
    }
    return service_template::SayHelloTo(res);
  }

  userver::storages::postgres::ClusterPtr pg_cluster_;
  std::shared_ptr<userver::storages::mysql::Cluster> mysql_cluster_;
  const MY_CACHE_POSTGRE* pg_cache_;
  const MY_CACHE_MYSQL* mysql_cache_;
};

} // namespace

std::string SayHelloTo(std::string_view name) {
  if (name.empty()) {
    name = "unknown user";
  }

  return fmt::format("Hello, {}!\n", name);
}

void AppendHello(userver::components::ComponentList &component_list) {
  component_list.Append<Hello>();
  component_list.Append<userver::components::Postgres>("postgres-db");
  component_list.Append<userver::components::PostgreCache<PostgresTrivialPolicy>>("pg-cache");
  component_list.Append<userver::storages::mysql::Component>("mysql-db");
  component_list.Append<userver::components::Secdist>("secdist");
  component_list.Append<userver::components::DefaultSecdistProvider>("default-secdist-provider");
  component_list.Append<userver::components::MySqlCache<MySQLTrivialPolicy>>("mysql-cache");
}

} // namespace service_template
