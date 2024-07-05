#include <sw/redis++/redis++.h>
#include <iostream>
#include <unordered_set>
#include <algorithm>
using namespace std;
using namespace sw::redis;

// cout << vector
template <typename T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &v)
{
    if (!v.empty())
    {
        os << '[';
        std::copy(v.begin(), v.end(), std::ostream_iterator<T>(os, ", "));
        os << "\b\b]"; // 删除末尾的", "
    }
    return os;
}
// cout << unordered_map
template <typename T, typename U>
std::ostream &operator<<(std::ostream &os, const std::unordered_map<T, U>&umap)
{
    os << '[';
    for (auto item : umap)
    {   
        os << "(" << item.first << "," << item.second << "),";
    }
    os << "\b]"; // 删除末尾的","
    return os;
}
// cout << unorderd_set
template <typename T>
std::ostream &operator<<(std::ostream &os, const std::unordered_set<T>&uset)
{
    os << '(';
    for (auto item : uset)
    {
        os << item << ",";
    }
    os << "\b)"; // 删除末尾的","
    return os;
}
int main()
{
    try
    {
        // Create an Redis object, which is movable but NOT copyable.
        auto redis = Redis("tcp://127.0.0.1:6379");
        /// ***** STRING commands *****
        redis.set("key", "val");
        // val is of type OptionalString. See 'API Reference' section fordetails.
        auto val = redis.get("key");
        if (val)
        {
          // Dereference val to get the returned value of std::stringtype.
           std::cout << *val << std::endl;
        } // else key doesn't exist.
        /// ***** LIST commands *****
        // std::vector<std::string> to Redis LIST.
        std::vector<std::string> vec = {"a", "b", "c"};
        redis.rpush("list", vec.begin(), vec.end());
        // Redis LIST to std::vector<std::string>.
        vec.clear();
        redis.lrange("list", 0, -1, std::back_inserter(vec));
        cout << "list: " << vec << endl;
        /// ***** HASH commands *****
        redis.hset("hash", "field", "val");
        // Another way to do the same job.
        redis.hset("hash", std::make_pair("field", "val"));
        // std::unordered_map<std::string, std::string> to Redis HASH.
        std::unordered_map<std::string, std::string> m ={
                {"field1", "val1"},
                {"field2", "val2"}};
        redis.hmset("hash", m.begin(), m.end());
        // Redis HASH to std::unordered_map<std::string, std::string>.
        m.clear();
        redis.hgetall("hash", std::inserter(m, m.begin()));
        cout << "hash:" << m << endl;
        // Get value only.
        // NOTE: since field might NOT exist, so we need to parse it toOptionalString.
        std::vector<OptionalString> vals;
        std::back_inserter(vals);
        /// ***** SET commands *****
        redis.sadd("set", "m1");
        // std::unordered_set<std::string> to Redis SET.
        std::unordered_set<std::string> set = {"m2", "m3"};
        redis.sadd("set", set.begin(), set.end());
        // Redis SET to std::unordered_set<std::string>.
        set.clear();

        redis.smembers("set", std::inserter(set, set.begin()));
        cout << "set:" << set << endl;
        if (redis.sismember("set", "m1"))
        {
            std::cout << "m1 exists" << std::endl;
        } // else NOT exist.
        /// ***** SORTED SET commands *****
        redis.zadd("sorted_set", "m1", 1.3);
        // std::unordered_map<std::string, double> to Redis SORTED SET.
        std::unordered_map<std::string, double> scores ={
                {"m2", 2.3},
                {"m3", 4.5}};
        redis.zadd("sorted_set", scores.begin(), scores.end());
        // Redis SORTED SET to std::vector<std::pair<std::string, double>>.
        // NOTE: The return results of zrangebyscore are ordered, if you save the results
        // in to `std::unordered_map<std::string, double>`, you'll lose theorder.
        std::vector<std::pair<std::string, double>> zset_result;
        redis.zrangebyscore("sorted_set",
        UnboundedInterval<double>{}, // (-inf, +inf)
        std::back_inserter(zset_result));
        // Only get member names:
        // pass an inserter of std::vector<std::string> type as outputparameter.
        std::vector<std::string> without_score;
        redis.zrangebyscore("sorted_set",
        BoundedInterval<double>(1.5, 3.4,
        BoundType::CLOSED),
        // [1.5, 3.4]
        std::back_inserter(without_score));
        // Get both member names and scores:
        // pass an back_inserter of std::vector<std::pair<std::string,double>> as output parameter.
        std::vector<std::pair<std::string, double>> with_score;
        redis.zrangebyscore("sorted_set",
        BoundedInterval<double>(1.5, 3.4,
        BoundType::LEFT_OPEN),
        // (1.5, 3.4]
        std::back_inserter(with_score));
        /// ***** SCRIPTING commands *****
        // Script returns a single element.
        auto num = redis.eval<long long>("return 1", {}, {});
        // Script returns an array of elements.
        std::vector<std::string> nums;

        // mset with TTL
        auto mset_with_ttl_script = R"(
        local len = #KEYS
        if (len == 0 or len + 1 ~= #ARGV) then return 0 end
        local ttl = tonumber(ARGV[len + 1])
        if (not ttl or ttl <= 0) then return 0 end
        for i = 1, len do redis.call("SET", KEYS[i], ARGV[i], "EX", ttl)
        end
        return 1
        )";
        // Set multiple key-value pairs with TTL of 60 seconds.
        auto keys = {"key1", "key2", "key3"};
        std::vector<std::string> args = {"val1", "val2", "val3", "60"};
        redis.eval<long long>(mset_with_ttl_script, keys.begin(),
        keys.end(),
        args.begin(), args.end());
        /// ***** Pipeline *****
        // Create a pipeline.
        auto pipe = redis.pipeline();

        /// ***** Transaction *****
        // Create a transaction.
        auto tx = redis.transaction();
        // Run multiple commands in a transaction, and get all replies.
        auto tx_replies = tx.incr("num0")
        .incr("num1")
        .mget({"num0", "num1"})
        .exec();
        // Parse reply with reply type and index.
        auto incr_result0 = tx_replies.get<long long>(0);
        auto incr_result1 = tx_replies.get<long long>(1);

        std::vector<OptionalString> mget_cmd_result;
        tx_replies.get(2, back_inserter(mget_cmd_result));
        /// ***** Generic Command Interface *****
        // There's no *Redis::client_getname* interface.
        // But you can use *Redis::command* to get the client name.
        val = redis.command<OptionalString>("client", "getname");
        if (val)
        {
            std::cout << *val << std::endl;
        }
        // Same as above.
        auto getname_cmd_str = {"client", "getname"};
        val = redis.command<OptionalString>(getname_cmd_str.begin(),getname_cmd_str.end());
        // There's no *Redis::sort* interface.
        // But you can use *Redis::command* to send sort the list.
        std::vector<std::string> sorted_list;
        redis.command("sort", "list", "ALPHA",std::back_inserter(sorted_list));
        // Another *Redis::command* to do the same work.
        auto sort_cmd_str = {"sort", "list", "ALPHA"};
        redis.command(sort_cmd_str.begin(), sort_cmd_str.end(),
        std::back_inserter(sorted_list));
        /// ***** Redis Cluster *****
        // Create a RedisCluster object, which is movable but NOT copyable.
        auto redis_cluster = RedisCluster("tcp://127.0.0.1:7000");
        // RedisCluster has similar interfaces as Redis.
        redis_cluster.set("key", "value");
        val = redis_cluster.get("key");
        if (val)
        {
            std::cout << *val << std::endl;
        } // else key doesn't exist.
        // Keys with hash-tag.
        redis_cluster.set("key{tag}1", "val1");
        redis_cluster.set("key{tag}2", "val2");
        redis_cluster.set("key{tag}3", "val3");
        std::vector<OptionalString> hash_tag_res;
        std::back_inserter(hash_tag_res);
    }
    catch (const Error &e)
    {
        // Error handling.
    }
    return 0;
}
