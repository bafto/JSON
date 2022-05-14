#ifndef JSON_H
#define JSON_H

#include <map>
#include <variant>
#include <vector>
#include <string>
#include <string_view>
#include <optional>

namespace json 
{
	typedef std::monostate Null;
	inline constexpr Null null = std::monostate();

	class Object;
	typedef struct Value Value;
	typedef std::vector<Value> Array;

	template<typename T>
	concept json_value =
		std::same_as<T, std::string> ||
		std::same_as<T, int> ||
		std::same_as<T, float> ||
		std::same_as<T, bool> ||
		std::same_as<T, Object> ||
		std::same_as<T, Array> ||
		std::same_as<T, Null>;

	class Object
	{
	public:
		Object() = default;
		Object(std::initializer_list<std::pair<std::string, Value>>);
		~Object() = default;

		Value& operator[](std::string str);
		const Value& operator[](std::string str) const;

		bool hasProperty(std::string str) const;
		std::string to_string(const std::string& seperator = "\t") const;
	public:
		std::map<std::string, Value> properties;
	};

	struct Value
	{
		enum class Type
		{
			Null,
			String,
			Int,
			Float,
			Bool,
			Object,
			Array
		};

		std::variant<
			Null,
			std::string,
			int,
			float,
			bool,
			Object,
			Array
		> value {null};

		constexpr Value() : value(null) {};
		Value(const std::string& val) : value(val) {};
		Value(const char* val) : value(std::string(val)) {};
		Value(const int& val) : value(val) {};
		Value(const float& val) : value(val) {};
		Value(const bool& val) : value(val) {};
		Value(const Object& val) : value(val) {};
		Value(const Array& val) : value(val) {};
		Value(const Null& val) : value(val) {};

		template<json_value T>
		T as() const;

		template<>
		std::string as<std::string>() const;
		template<>
		int as<int>() const;
		template<>
		float as<float>() const;
		template<>
		bool as<bool>() const;
		template<>
		Object as<Object>() const;
		template<>
		Array as<Array>() const;
		template<>
		Null as<Null>() const;

		explicit operator std::string() const;
		explicit operator int() const;
		explicit operator float() const;
		explicit operator bool() const;
		explicit operator Object() const;
		explicit operator Array() const;
		explicit operator Null() const;

		std::string to_string() const;
		Type type() const;
	};

	template<json_value T>
	T fromJsonValue(const Value& val);

	template<>
	std::string fromJsonValue<std::string>(const Value& val);
	template<>
	int fromJsonValue<int>(const Value& val);
	template<>
	float fromJsonValue<float>(const Value& val);
	template<>
	bool fromJsonValue<bool>(const Value& val);
	template<>
	Object fromJsonValue<Object>(const Value& val);
	template<>
	Array fromJsonValue<Array>(const Value& val);
	template<>
	Null fromJsonValue<Null>(const Value& val);

	template<json_value T>
	Value toJsonValue(const T& val);

	template<>
	Value toJsonValue<std::string>(const std::string& val);
	template<>
	Value toJsonValue<int>(const int& val);
	template<>
	Value toJsonValue<float>(const float& val);
	template<>
	Value toJsonValue<bool>(const bool& val);
	template<>
	Value toJsonValue<Object>(const Object& val);
	template<>
	Value toJsonValue<Array>(const Array& val);
	template<>
	Value toJsonValue<Null>(const Null& val);

	std::optional<Value> parse(std::string_view str);
} // namespace json

std::ostream& operator<<(std::ostream&, json::Value);

#endif // JSON_H