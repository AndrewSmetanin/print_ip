#include <iostream>
#include <vector>
#include <list>
#include <tuple>

//<Наглый копипаст>
template <typename T>
struct has_const_iterator
{
private:
    typedef char one;
    typedef struct
    {
        char array[2];
    } two;

    template <typename C>
    static one test(typename C::const_iterator *);
    template <typename C>
    static two test(...);

public:
    static const bool value = sizeof(test<T>(0)) == sizeof(one);
    typedef T type;
};

template <typename T>
struct has_begin_end
{
    struct Dummy
    {
        typedef void const_iterator;
    };
    typedef typename std::conditional<has_const_iterator<T>::value, T, Dummy>::type TType;
    typedef typename TType::const_iterator iter;

    struct Fallback
    {
        iter begin() const;
        iter end() const;
    };
    struct Derived : TType, Fallback
    {
    };

    template <typename C, C>
    struct ChT;

    template <typename C>
    static char (&f(ChT<iter (Fallback::*)() const, &C::begin> *))[1];
    template <typename C>
    static char (&f(...))[2];
    template <typename C>
    static char (&g(ChT<iter (Fallback::*)() const, &C::end> *))[1];
    template <typename C>
    static char (&g(...))[2];

    static bool const beg_value = sizeof(f<Derived>(0)) == 2;
    static bool const end_value = sizeof(g<Derived>(0)) == 2;
};

template <typename T>
struct is_container
{
    static const bool value = has_const_iterator<T>::value &&
                              has_begin_end<T>::beg_value && has_begin_end<T>::end_value;
};
//</Наглый копипаст>


// <Копипаст с изменениями>
template <typename T>
constexpr size_t tuple_size_v = std::tuple_size<T>::value;

template<std::size_t> struct int_{};

template <class Tuple, size_t Pos>
std::ostream& print_tuple(std::ostream& out, const Tuple& t, int_<Pos> ) {
  out << std::get< tuple_size_v<Tuple>-Pos >(t) << '.';
  return print_tuple(out, t, int_<Pos-1>());
}

template <class Tuple>
std::ostream& print_tuple(std::ostream& out, const Tuple& t)
{
    return print_tuple(out,t,int_<tuple_size_v<Tuple>>{});
}


template <class Tuple>
std::ostream& print_tuple(std::ostream& out, const Tuple& t, int_<1> ) {
  return out << std::get<std::tuple_size<Tuple>::value-1>(t) <<std::endl;
}

//</Копипаст с изменениями>

template <class T, class... Args>
struct are_all_same;

template <class T>
struct are_all_same<T>:std::true_type{};

template <class T>
struct are_all_same<T,T>:std::true_type{};

template <class T, class U>
struct are_all_same<T,U>:std::false_type{};

template <class T, class... Args>
struct are_all_same<T,T,Args...>:are_all_same<T,Args...>{};

template <class T, class U, class... Args>
struct are_all_same<T,U,Args...>:std::false_type{};


template <class T, class... Args>
constexpr bool are_all_same_v = are_all_same<T,Args...>::value;

template <typename T>
constexpr bool is_container_v = is_container<T>::value;

template <typename T1, typename T2>
constexpr bool is_same_v = std::is_same<T1, T2>::value;

template <typename T>
constexpr bool is_integral_v = std::is_integral<T>::value;

template <class T>
std::enable_if_t<is_integral_v<T>, T> print(T integral)
{
    size_t byteLength = sizeof(integral);
    for (size_t i = 0; i < byteLength - 1; ++i)
    {
        int currentByte = integral >> ((byteLength - i - 1) * 8) & 0xFF;
        std::cout << currentByte << '.';
    }
    std::cout << (integral & 0xFF) << std::endl;
    return integral;
}

template <class T>
std::enable_if_t<is_same_v<T, std::string>, T> print(const T &str)
{
    std::cout << str << std::endl;
    return str;
}

template< class T >
constexpr bool is_floating_point_v = std::is_floating_point<T>::value;

template <class T>
std::enable_if_t<is_container_v<T> && (!is_same_v<T, std::string>), T> print(const T &container)
{
    size_t length = container.size();
    auto it = container.begin();
    for (size_t i = 0; i<length-1; ++i)
    {
        std::cout<<(*it)<<".";
        ++it;
    }
    std::cout<<(*it)<<std::endl;
    return container;
}


template <class... Args>
std::enable_if_t<are_all_same_v<Args...>,std::tuple<Args...>> print(std::tuple<Args...> tuple_)
{
    print_tuple(std::cout,tuple_);
    return tuple_;
}

int main(int, char **)
{
    print(char(-1));
    print(short(0));
    print(int(2130706433));
    print(8875824491850138409LL);

    std::string s("123.456.789.484");
    
    const size_t WIDTH = 4;
    std::vector<std::string> v = {"192","168","0","1"};
    

    print(s);
    print(v);
    std::list<unsigned int> list = {2130706433,0b00000000011111111,0xFFFFFFFF,0};
    print(list);

    auto t = std::tuple<int,int,int,int>(1,2,3,4);
    //print<std::tuple<int,int,int,int>,int,int,int,int>(t);
    
    print(t);

    return 0;
}