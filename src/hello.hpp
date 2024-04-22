#pragma once

#include <cstdint>
#include <string>
#include <string_view>

#include <userver/components/component_list.hpp>
#include <userver/storages/postgres/cluster_types.hpp>

namespace service_template {

struct MyStructure {
  int id = 0;
  std::string name{};

  int get_id() const { return id; }
};

struct MyStructureMYSQL {
  std::uint64_t id = 0;
  std::optional<std::string> name{};

  int get_id() const { return id; }
};


struct PostgresTrivialPolicy {
  static constexpr std::string_view kName = "my-pg-cache";
 
  using ValueType = MyStructure;

  static constexpr auto kKeyMember = &MyStructure::id;
 
  static constexpr const char* kQuery = "SELECT id, name FROM example_table;";
 
  static constexpr const char* kUpdatedField = "";
  static constexpr auto kClusterHostType =
      userver::storages::postgres::ClusterHostType::kMaster;

  using CacheContainer = std::unordered_map<int, MyStructure>;
};

struct MySQLTrivialPolicy {
  static constexpr std::string_view kName = "mysql-cache";
 
  using ValueType = MyStructureMYSQL;

  static constexpr auto kKeyMember = &MyStructureMYSQL::id;
 
  static constexpr const char* kQuery = "SELECT id, name FROM example_table;";
 
  static constexpr const char* kUpdatedField = "";
  static constexpr auto kClusterHostType =
      userver::storages::postgres::ClusterHostType::kMaster;

  using CacheContainer = std::unordered_map<std::uint64_t, MyStructureMYSQL>;
};

std::string SayHelloTo(std::string_view name);

void AppendHello(userver::components::ComponentList &component_list);

} // namespace service_template
