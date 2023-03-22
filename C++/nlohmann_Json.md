## You

详细讲述nlohmann/json的api

## Genie

nlohmann/json是一个用C++编写的JSON库，它支持现代C++特性，如lambda表达式、范围for循环和模板元编程。以下是一些重要的API：

- `json` 类型：这是表示JSON对象的主要类型。它可以存储字符串、数字、布尔值、数组和嵌套JSON对象。示例代码：

  ```c++
  #include <nlohmann/json.hpp>
  using json = nlohmann::json;

  json j = {
      {"name", "John"},
      {"age", 30},
      {"isMale", true},
      {"hobbies", {"reading", "swimming"}}
  };

  // 获取属性
  std::string name = j["name"];
  int age = j["age"];
  bool isMale = j["isMale"];
  std::vector<std::string> hobbies = j["hobbies"];
  ```

- `parse()` 函数：将JSON字符串解析为 `json` 对象。示例代码：

  ```c++
  std::string jsonString = R"(
      {
        "name": "Alice",
        "age": 25,
        "isMale": false,
        "hobbies": ["music", "travel"]
      }
  )";

  json j = json::parse(jsonString);

  std::string name = j["name"];
  int age = j["age"];
  bool isMale = j["isMale"];
  std::vector<std::string> hobbies = j["hobbies"];
  ```

- `dump()` 函数：将 `json` 对象转换为JSON字符串。示例代码：

  ```c++
  json j = {
      {"name", "Bob"},
      {"age", 20},
      {"isMale", true},
      {"hobbies", {"sports", "movies"}}
  };

  std::string jsonString = j.dump();
  ```

- 迭代器：可以像操作STL容器一样遍历JSON对象和数组。示例代码：

  ```c++
  json j = {
      {"name", "Charlie"},
      {"age", 35},
      {"isMale", true},
      {"hobbies", {"painting", "cooking"}}
  };

  // 遍历属性
  for (json::iterator it = j.begin(); it != j.end(); ++it) {
      std::cout << it.key() << ": " << it.value() << '\n';
  }

  // 遍历数组
  for (auto& hobby : j["hobbies"]) {
      std::cout << hobby << '\n';
  }
  ```

- 异常类：当对 `json` 对象使用无效操作时，会抛出异常。有三个异常类：`out_of_range`、`type_error` 和 `parse_error`。例如：

  ```c++
  try {
      std::string unknown = j["unknown_property"]; // 抛出 out_of_range 异常
  } catch (const std::out_of_range& e) {
      std::cerr << "Error: " << e.what() << '\n';
  }

  json invalidJson = json::parse("invalid JSON"); // 抛出 parse_error 异常
  ```

这只是nlohmann/json库提供的一小部分API。更多信息，请查看官方文档：https://github.com/nlohmann/json/blob/develop/doc/examples/README.md