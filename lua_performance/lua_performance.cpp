#include "lua_performance.h"
#include <map>
#include <set>
#include <vector>
#include <string>
#include <boost/format.hpp>

#include <boost/chrono.hpp>

#define RETURN_NIL do{lua_pushnil(L);return 1;}while(false);

static const char * performance_register_name = "213xzfsd";

#define i32 __int32
#define i64 __int64

static const i32 type_1 = 1;
static const i32 type_2 = 2;

static i32 hash(const char *key,int length)
{
	int hash = 0;
	int i;
	for (i = 0; i < length; ++i) hash = 33 * hash + key[i];
	return hash;
}

namespace lua_performance
{
	namespace{

		struct keeper
		{
			keeper(lua_State *_L)
				:L(_L), top(lua_gettop(L))
			{
				
			}
			~keeper()
			{
				lua_settop(L, top);
			}

			lua_State *L;
			int top;
		};
#define keep_guard keeper k_dffdsfasdf(L)

		static boost::chrono::high_resolution_clock::time_point getHighTime()
		{
			return boost::chrono::high_resolution_clock::now();
		}

		class node_key
		{
		public:
			node_key(int _line_define,
				int _last_line_define,
				int _on_call_line,
				const std::string &_short_src,
				bool _istailcall)
				:line_define(_line_define), last_line_define(_last_line_define), on_call_line(_on_call_line), short_src(_short_src), istailcall(_istailcall)
			{
				
			}
			int line_define;
			int last_line_define;
			int on_call_line;
			std::string short_src;
			bool istailcall;
		};

		struct type_1_compare
		{
			bool operator() (const node_key &left, const node_key &right)
			{
				if (left.last_line_define != right.last_line_define)
				{
					return left.last_line_define < right.last_line_define;
				}
				else
				if (left.istailcall != right.istailcall)
				{
					return !left.istailcall;
				}

				return false;
			}
		};

		struct type_2_compare
		{
			bool operator() (const node_key &left, const node_key &right)
			{
				if (left.last_line_define != right.last_line_define)
				{
					return left.last_line_define < right.last_line_define;
				}

				return false;
			}
		};

		struct node_data
		{
			node_data()
			:count(0),begin_point(),sum_time(0),sum_water_duration(0)
			{

			}
			int count;
			boost::chrono::high_resolution_clock::time_point begin_point;
			boost::chrono::nanoseconds sum_time;
			boost::chrono::nanoseconds sum_water_duration;
		};

		struct node
		{
			node():key(nullptr){}
			node_data data;
			const node_key *key;
			std::map<node_key, node, type_1_compare> child_map;
		};

		// 每个lua_State有一个单独的performance_data_manager
		class Performance_data_manager
		{
		public:
			Performance_data_manager(lua_State *_L)
				:L(_L), expand_root(nullptr), performance_test_type(0)
			{
				refresh_data();
			}
			~Performance_data_manager()
			{
				if (expand_root)
				{
					delete expand_root;
					expand_root = nullptr;
				}
			}

		public:
			static Performance_data_manager * instance(lua_State *_L)
			{
				keeper k(_L);

				lua_getfield(_L, LUA_REGISTRYINDEX, class_name);
				if (lua_isnil(_L, -1))
				{
					auto *p = new Performance_data_manager(_L);
					auto void_p = lua_newuserdata(_L, sizeof(p));
					memcpy(void_p, &p, sizeof(p));
					{
						lua_newtable(_L);
						{
							lua_pushcfunction(_L, Performance_data_manager::gc);
							lua_setfield(_L, -2, "__gc");
						}
						lua_setmetatable(_L, -2);
					}
					lua_setfield(_L, LUA_REGISTRYINDEX, class_name);
					return p;
				}
				auto p = lua_touserdata(_L, -1);
				return *static_cast<Performance_data_manager **>(p);
			}

			static int gc(lua_State *_L)
			{
				auto _this = *static_cast<Performance_data_manager **>(lua_touserdata(_L, 1));
				delete _this;

				lua_pushnil(_L);
				return 1;
			}

			void refresh_data()
			{
				if (expand_root)
				{
					delete expand_root;
				}

				expand_root = new node;

				node_stack.clear();
				node_stack.push_back(expand_root);
			}

			void push_stack(node *node)
			{
				node_stack.push_back(node);
			}

			node *get_top()
			{
				if (!node_stack.empty())
				{
					return node_stack.back();
				}
				
				return nullptr;
			}

			void pop_stack()
			{
				node_stack.pop_back();
			}

		private:
			static const char *class_name;
		private:
			lua_State *L = nullptr;
		public:
			int ref_count = 0;

			node *expand_root;
			std::map<node_key, node_data, type_2_compare> call_info_map;
			std::vector<node *> node_stack;

			i32 performance_test_type;
		};

		const char *Performance_data_manager::class_name = "hsdkfhsk";

		class Performance_Process
		{
		public:
			Performance_Process(lua_State *_L)
			:L(_L)
			{
				
			}

			Performance_data_manager *get_global_data()
			{
				return Performance_data_manager::instance(L);
			}

			void befor_call();
			void call();
			void after_call();
			int return_result();
		private:
			void attach_hook();
			void detach_hook();
			void send_performance_data();
		private:
			lua_State *L;
			int ret_count = 0;
		};

		void Performance_Process::befor_call()
		{
			keep_guard;

			auto data = get_global_data();
			if (data->ref_count == 0)
			{
				if (lua_isinteger(L, lua_upvalueindex(4)))
				{
					data->performance_test_type = (i32)lua_tointeger(L, lua_upvalueindex(4));
				}
				else
				{
					data->performance_test_type = type_1;
				}

				attach_hook();
			}

			++data->ref_count;
		}

		void Performance_Process::call()
		{
			int arg_count = lua_gettop(L);

			lua_pushvalue(L, lua_upvalueindex(3)); // the safe_call_function
			lua_pushvalue(L, lua_upvalueindex(1)); // your function
			lua_rotate(L, 1, 2);

			// current stack:
			//    safe_call function
			//    args...
			//    original function
			lua_call(L, arg_count + 1, LUA_MULTRET);

			// now, stack are all parameters
			ret_count = lua_gettop(L);
		}

		void Performance_Process::after_call()
		{
			keep_guard;
			auto data = get_global_data();
			--data->ref_count;
			if (data->ref_count == 0)
			{
				detach_hook();
				send_performance_data();
				data->refresh_data();
				
				data->performance_test_type = 0;
			}
		}

		int Performance_Process::return_result()
		{
			return ret_count;
		}

		void hook(lua_State *L, lua_Debug *ar); // forward declaration
		void Performance_Process::attach_hook()
		{
			lua_sethook(L, hook, LUA_MASKCALL | LUA_MASKRET, 0);			
		}

		void Performance_Process::detach_hook()
		{
			lua_sethook(L, nullptr, 0, 0);
		}

		static void data_to_table_type_1(lua_State *L, node *node)
		{
			lua_createtable(L, node->child_map.size(), 2);

			keep_guard;

			auto *key = node->key;
			if (key)
			{
				const char *fmt = "%s:%d-%d";
				if (key->istailcall)
				{
					fmt = "*%s:%d-%d";
				}
				auto key_str = (boost::format(fmt) % key->short_src %key->line_define %key->last_line_define).str();
				lua_pushlstring(L, key_str.c_str(), key_str.length());
				lua_setfield(L, -2, "key");
			}
			else
			{
				lua_pushstring(L, "<----enter function---->");
				lua_setfield(L, -2, "key");
			}

			{
				lua_createtable(L, 0, 2);
				{
					lua_pushinteger(L, node->data.count);
					lua_setfield(L, -2, "count");
					lua_pushinteger(L, node->data.sum_time.count());
					lua_setfield(L, -2, "sum_time");
				}
				lua_setfield(L, -2, "value");
			}

			int count = 0;
			for (auto &pair : node->child_map)
			{
				keep_guard;
				++count;

				data_to_table_type_1(L, &pair.second);
				lua_rawseti(L, -2, count);
			}
		}

		static void data_to_table_type_2_detail(lua_State *L, node *node, std::map < node_key, node_data, type_2_compare> &m)
		{
			if (!node->key)
			{
				return;
			}

			auto it = m.find(*node->key);
			if (it == m.end())
			{
				it = m.insert({ *node->key, {} }).first;
			}

			
		}

		static void data_to_table_type_2(lua_State *L, node *node)
		{
			std::map < node_key, node_data, type_2_compare> m;
			data_to_table_type_2_detail(L, node, m);
		}

		void Performance_Process::send_performance_data()
		{
			keep_guard;
			lua_pushvalue(L, lua_upvalueindex(3)); // push stack upvalue 3:safe_call_function
			int error_index = lua_gettop(L);
			lua_pushvalue(L, lua_upvalueindex(2)); // push stack upvalue 2:performance_receiver(data, sum_time)
			auto *data = get_global_data();

			data_to_table_type_1(L, data->expand_root);

			lua_pushinteger(L, data->expand_root->data.sum_time.count()); // nanoseconds, 1:10^9
			lua_call(L, 3, 0);
		}

		

		node * get_location_in_tree(node_key &key, Performance_data_manager *data, bool could_create_new_node)
		{
			auto &cur_node = *data->node_stack.back();
			auto &child_map = cur_node.child_map;
			auto it = child_map.find(key);
			
			if (it == child_map.end())
			{
				auto it = child_map.insert({ key, {} });
				it.first->second.key = &it.first->first;
				return &it.first->second;
			}
			else
			{
				return &it->second;
			}
		}

		node_key generate_node_key(lua_State *L, lua_Debug *self_ar, bool istailcall)
		{
			lua_getinfo(L, "Sn", self_ar);

			lua_Debug parent_ar;
			lua_getstack(L, 1, &parent_ar);
			lua_getinfo(L, "l", &parent_ar);

			return node_key(self_ar->linedefined, self_ar->lastlinedefined, parent_ar.currentline, self_ar->short_src, istailcall);
		}

		static void on_call(lua_State *L, lua_Debug *ar)
		{
			auto begin_point = getHighTime();
			
			auto data = Performance_data_manager::instance(L);

			node_key key(generate_node_key(L, ar, false));
			auto self_node = get_location_in_tree(key, data, true);
			auto &n_data = self_node->data;
			++n_data.count;
			n_data.begin_point = begin_point;
			data->push_stack(self_node);
		}

		static void on_tail_call(lua_State *L, lua_Debug *ar)
		{
			auto begin_point = getHighTime();
			auto data = Performance_data_manager::instance(L);

			node_key key(generate_node_key(L, ar, true));
			auto self_node = get_location_in_tree(key, data, true);
			auto &n_data = self_node->data;
			++n_data.count;
			n_data.begin_point = begin_point;
			data->push_stack(self_node);
		}

		static void on_ret(lua_State *L, lua_Debug *ar)
		{
			lua_getinfo(L, "n", ar);
			auto data = Performance_data_manager::instance(L);
			//node_key key(generate_node_key(L, ar, false));
			auto now = getHighTime();
			for (auto *node = data->get_top(); node != nullptr ;node=data->get_top())
			{
				data->pop_stack();
				
				node->data.sum_time += now - node->data.begin_point;

				if (!node->key->istailcall)
				{
					break;
				}
				else
				{
					continue;
				}
			}
		}

		static void hook(lua_State *L, lua_Debug *ar)
		{
			switch(ar->event){
			case LUA_HOOKCALL:
				on_call(L, ar);
				break;
			case LUA_HOOKTAILCALL:
				on_tail_call(L, ar);
				break;
			case LUA_HOOKRET:
				on_ret(L, ar);
				break;
			}
		}
	}

	

	// upvalue 1:func  your function
	// upvalue 2:performance_receiver(data, sum_time)
	// upvalue 3:safe_call_function
	// upvalue 4:performance test type (default:1)
	static int proxy_func(lua_State *L)
	{
		Performance_Process processer(L);
		
		processer.befor_call();
		processer.call();
		processer.after_call();

		return processer.return_result();
	}

	static int default_safe_call(lua_State *L)
	{
		if (lua_pcall(L, lua_gettop(L) - 1, LUA_MULTRET, 0) != LUA_OK)
		{
			lua_getglobal(L, "print");
			lua_pushvalue(L, -2);
			lua_call(L, 1, 1);
			return 1;
		}
		return lua_gettop(L) - 0;
	}

	// wrap(func, receiver, safe_call_func)
	int wrap(lua_State *L)
	{
		// the #1 arg should be a function(original func)
		luaL_checkany(L, 1);

		// the #2 arg should be a function(performance_receiver)
		luaL_checkany(L, 2);
		if (!lua_isfunction(L, 2))
		{
			return luaL_error(L, "must give me a performance_data_receiver function !");
		}

		if (lua_isnone(L, 3))
		{
			lua_pushcfunction(L, default_safe_call);
		}

		else if (!lua_isfunction(L, 3))
		{
			lua_pushcfunction(L, default_safe_call);
			lua_replace(L, 3);
		}

		lua_pushcclosure(L, proxy_func, 3);
		return 1;
	}

}