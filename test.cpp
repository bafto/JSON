#include <iostream>
#include <filesystem>
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
			"goodGirl": true,
			"friend": {
					"name": "julia",
					"age": 14,
					"goodGirl": true
				}
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

	std::cout << json::parse(jsonstr).value_or(("invalid json")).to_string("  ") << "\n\n"
		<< json::parseFile("test.json").value_or(("unable to parse file")).to_string("  ") << "\n\n"
		<< json::parse(jsonarr).value_or(("invalid json")).to_string("  ");

	return 0;
}