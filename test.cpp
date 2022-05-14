#include <iostream>
#include "json.h"

struct Dog
{
	std::string name;
	int age;
	bool goodBoi;

	bool operator==(const Dog& rhs) const = default;

	Dog(std::string name, int age, bool goodBoi)
		:
		name(name),
		age(age),
		goodBoi(goodBoi)
	{}
	Dog(const json::Object& obj)
		:
		name(obj["name"]),
		age(obj["age"]),
		goodBoi(obj["goodBoi"])
	{}

	json::Object toJson() const
	{
		return {
			{"name", name},
			{"age", age},
			{"goodBoi", goodBoi}
		};
	}
};

int main() 
{
	// how should the api look:
	/*
	json::Object obj = json::toJson(some_object);
	Some_Type some_other_object = json::fromJson(obj);

	obj["property"] = some_value;
	std::string str = obj["string_property"];
	int i = obj["int_property"];
	*/

	/*Dog dog1("tim", 10, true);
	json::Object obj = dog1.toJson();
	json::Object obj2 = obj;
	Dog dog2(obj2);

	std::cout << (dog1 == dog2);
	if (obj.hasProperty("name")) {
		std::cout << "\n" << obj;
	}*/

	json::Object obj1 = {
		{"name", "tom"},
		{"age", 17},
		{"male", true},
	};

	json::Object obj2 = {};

	json::Array objArr = {
		obj1,
		obj2,
	};

	std::string jsonstr = R"(
	{
		"name": "tim",
		"age": 15,
		"goodBoi"   :	true,
		"sister": {
			"name": "jenny"  ,
			"age": 13,
			"goodGirl": true
		},
		"orphan": {}
	}
)";

	std::string jsonarr = R"(
	[
		{
			"name": "tim",
			"age": 15,
			"goodBoi"   :	true,
			"sister": {
				"name": "jenny"  ,
				"age": 13,
				"goodGirl": true
			},
			"orphan": {}
		},
		12,
		22.456,
		"hello",
		[
			"hello",
			true
		]
	]
)";

	std::cout << json::parse(jsonstr).value_or(("invalid json")) << "\n\n"
		<< json::parse(jsonarr).value_or(("invalid json"));

	return 0;
}