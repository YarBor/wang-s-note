nlohmann/json 是一个方便易用的 C++ JSON 库，可以实现将 JSON 数据与 C++ 对象相互转换的功能，支持常见的 JSON 数据类型，例如对象、数组、字符串等。
本文将针对 nlohmann/json 的集合和字典功能进行讲解，并介绍如何从 JSON 对象中解析特定字段。

> 文末有实体代码 将创建一个足够复杂的json字段

用于了解类型是怎么加入和创建的

---

> 这里讲述了复杂类型如何进行创建
> 简单类型(简单kv) 可以看demo
## 集合

在 nlohmann/json 中，可以使用 `nlohmann::json` 类型来表示 JSON 数组。以下是一些创建和操作 JSON 数组的示例：

```cpp
// 创建空的 JSON 数组
nlohmann::json json_array = nlohmann::json::array();

// 添加元素到 JSON 数组
json_array.push_back("element1");
json_array.push_back(42);

// 遍历 JSON 数组并输出每个元素
for (const auto& element : json_array) {
    std::cout << "Element: " << element << std::endl;
}
```

在上述示例中，我们首先使用 `nlohmann::json::array()` 创建了一个空的 JSON 数组 `json_array`。然后，我们分别使用 `push_back()` 方法向该数组中添加一个字符串元素 `"element1"` 和一个整数元素 `42`。最后，我们使用 for 循环遍历数组中的所有元素，并输出它们的值。
```json
Element: "element1"
Element: 42
[
    "element1",
    42
]

```
## 字典

在 nlohmann/json 中，可以使用 `nlohmann::json` 类型来表示 JSON 对象，它通常被认为是一个字典。以下是一些创建和操作 JSON 对象的示例：

```cpp
// 创建空的 JSON 对象
nlohmann::json json_object = nlohmann::json::object();

// 添加键值对到 JSON 对象
json_object["key1"] = "value1";
json_object["key2"] = 42;

// 遍历 JSON 对象并输出每个键和值
for (auto& element : json_object.items()) {
    std::cout << "Key: " << element.key() << ", Value: " << element.value() << std::endl;
}
```

在上述示例中，我们首先使用 `nlohmann::json::object()` 创建了一个空的 JSON 对象 `json_object`。然后，我们使用 `[]` 运算符向该对象中添加两个键值对 `"key1": "value1"` 和 `"key2": 42`。最后，我们使用 for 循环遍历字典中的所有键值对，并输出它们的键和值。
```json
{
    "key1": "value1",
    "key2": 42
}
```
## 组合构造

除了单独创建数组或字典之外，nlohmann/json 还支持将它们组合起来以创建更复杂的数据结构。例如，以下示例演示如何使用 JSON 对象和数组嵌套来创建具有多层次结构的 JSON 数据：

```cpp
// 创建一个具有多层结构的 JSON 对象
nlohmann::json json_data;

// 在 JSON 对象中添加一个名为 "name" 的字符串属性
json_data["name"] = "John Doe";

// 在 JSON 对象中添加一个名为 "age" 的整数属性
json_data["age"] = 30;

// 在 JSON 对象中添加一个名为 "addresses" 的 JSON 数组
json_data["addresses"] = nlohmann::json::array();

// 创建一个具有多层结构的 JSON 对象，并添加到数组中
nlohmann::json address1;
address1["street"] = "123 Main St";
address1["city"] = "Anytown";
address1["state"] = "CA";

// 添加第一个地址对象到数组中
json_data["addresses"].push_back(address1);

// 创建另一个具有多层结构的 JSON 对象，并添加到数组中
nlohmann::json address2;
address2["street"] = "456 Oak Ave";
address2["city"] = "Othertown";
address2["state"] = "NY";

// 添加第二个地址对象到数组中
json_data["addresses"].push_back(address2);

// 将 JSON 对象转换为字符串并输出
std::string json_string = json_data.dump();
std::cout << json_string << std::endl;
```

在上述示例中，我们首先创建了一个空的 JSON 对象 `json_data`。然后，我们向该对象中添加一个名为 `"name"` 的字符串属性和一个名为 `"age"` 的整数属性。接着，我们通过将 `nlohmann::json::array()` 添加到对象中来创建一个名为 `"addresses"` 的 JSON 数组。

我们然后创建两个具有多层结构的 JSON 对象，分别代表不同的地址，并使用上述方法将它们添加到 `"addresses"` 数组中。最后，我们使用 `dump()` 方法将 JSON 对象转换为字符串，并将其输出到控制台上。

```json
{
    "addresses": [
        {
            "city": "Anytown",
            "state": "CA",
            "street": "123 Main St"
        },
        {
            "city": "Othertown",
            "state": "NY",
            "street": "456 Oak Ave"
        }
    ],
    "age": 30,
    "name": "John Doe"
}
```
## 解析 JSON 对象

设置好 `nlohmann::json` 对象以存储 JSON 数据后，您可以使用 `[]` 运算符访问特定字段。例如，假设我们有以下 JSON 数据：

```json
{
    "name": "John Doe",
    "age": 30,
    "addresses": [
        {
            "street": "123 Main St",
            "city": "Anytown",
            "state": "CA"
        },
        {
            "street": "456 Oak Ave",
            "city": "Othertown",
            "state": "NY"
        }
    ]
}
```

要访问 `"name"` 字段，请使用以下代码：

```cpp
// 解析 JSON 字符串并将其存储在 json_data 对象中
nlohmann::json json_data = nlohmann::json::parse(json_string);

// 获取 "name" 字段的值并将其输出
std::string name = json_data["name"];
std::cout << "Name: " << name << std::endl;
```

在上述示例中，我们首先使用 `nlohmann::json::parse()` 方法解析 JSON 字符串，并将其存储在 `json_data` 对象中。然后，我们使用 `[]` 运算符获取字段名为 `"name"` 的值，并将其赋值给一个字符串变量 `name`。最后，我们输出该变量的值到控制台上。

对于数组，可以使用 `for` 循环和 `size()` 方法遍历其中的每个元素，例如：

```cpp
// 遍历 "addresses" 数组并输出每个地址的城市和州
for (const auto& address : json_data["addresses"]) {
    std::string city = address["city"];
    std::string state = address["state"];
    std::cout << "City: " << city << ", State: " << state << std::endl;
}
```

在上述示例中，我们首先使用 `for` 循环遍历 JSON 数据中 `"addresses"` 数组中的每个元素。然后，我们对于每个元素使用 `[]` 运算符获取其 `"city"` 和 `"state"` 字段的值，并将这些值分别赋给字符串变量 `city` 和 `state`。最后，我们输出这些变量的值到控制台上。

<!-- nlohmann/json 是一个开源的 C++ JSON 库，可以轻松地将 JSON 数据和 C++ 对象相互转换。它提供了一种快速、方便、灵活的方式来处理 JSON 数据。 -->

<!-- 好的，以下是我用nlohmann::json实现以及模拟创建上述Json的过程。 -->

---
## 创建一个足够复杂的json 
```json
{
    "num": 999999,
    "data": [
        0,
        true,
        null,
        [
            1,
            {
                "msg": "Hello World",
                "time": "2021-10-01"
            },
            "2",
            [
                3,
                4,
                5
            ],
            "6"
        ],
        {
            "qwer": [
                "ewasd",
                7,
                {
                    "nums": [
                        8,
                        true,
                        []
                    ]
                }
            ],
            "json": "YarBor is awesome!",
            "book": {
                "name1": [
                    "price1",
                    "date",
                    132,
                    {
                        "a": "A"
                    }
                ],
                "1423": {
                    "1234": "qwer"
                }
            }
        }
    ]
}
这为目标json

```

---

该实现代码

```cpp
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

int main()
{
    // 创建一个空的json对象
    json j;

    j.["num"] = 999999;

    // 创建数组data，并加入元素0, true, null
    json data = json::array();
    data.push_back(0);
    data.push_back(true);
    data.push_back(nullptr);

    // 创建数组nestedArray，并加入元素1, object, string2, nestedArray2, string6
    json nestedArray = { 1 };
    json object;
    object["msg"] = "Hello World";
    object["time"] = "2021-10-01";
    nestedArray.push_back(object);
    nestedArray.push_back("2");
    json nestedArray2 = { 3, 4, 5 };
    nestedArray.push_back(nestedArray2);
    nestedArray.push_back("6");

    // 把嵌套后的数组放到data中
    data.push_back(nestedArray);

    // 创建包含键值对的对象
    json obj = {
        {"qwer", {"ewasd", 7, {{"nums", {8, true, {}}}}}},
        {"json", "YarBor is awesome!"}
    };

    // 把对象放到data中
    data.push_back(obj);

    // 把data数组放到根级别的对象中
    j["data"] = data;

    // 将JSON打印出来
    std::cout << j.dump(4) << std::endl;

    return 0;
}
```

我们首先创建了一个空的JSON对象，然后创建了一个空的数组data。
用push_back()方法向data数组中添加0、true和null。
创建一个nestedArray数组，并向其中添加1、一个object对象、"2"、nestedArray2数组和"6"。
然后向data数组中添加了这个nestedArray数组和包含键值对的obj对象。

最后我们用j["data"] = data将数组data放入根级别的JSON对象中，并用std::cout输出整个JSON字符串。

---
## 简便创建 json
```cpp

    json obj1 = {
        {"qwer", {"ewasd", 7, {{"nums", {8, true, {}}}}}},
        {"json", "YarBor is awesome!"},
        {"book", {
            {"name1", {"price1", "date", 132, {{"a", "A"}}}},
            {"1423", {{"1234", "qwer"}}}
        }}
    };

```

`std::cout << obj1.dump(4) << std::endl;`后的结果是 
```json
{
    "book": {
        "1423": {
            "1234": "qwer"
        },
        "name1": [
            "price1",
            "date",
            132,
            {
                "a": "A"
            }
        ]
    },
    "json": "YarBor is awesome!",
    "qwer": [
        "ewasd",
        7,
        {
            "nums": [
                8,
                true,
                null
            ]
        }
    ]
}
```