local f = require 'lua_test'

local log = function(...)
	print(...)
end

local function dump(node, block)
	log(string.rep('  ', block).. node.key, string.format("count:%d, sum_time:%fms", node.value.count, node.value.sum_time/1000000))
	for i = 1, #node do
		dump(node[i], block + 1)
	end
end

local function do_dump(data)
	dump(data,0)
end

local err_func = function(err)
	print(err)
	print(debug.traceback())
end

local safe_call = function(func, ...)
	local res = nil
	xpcall(function(...)
		res = func(...)
	end, err_func, ...)

	return res
end

local wrapper = performance_wrap(f, function(data, sum_time)
	do_dump(data)
end, safe_call, 1)

xpcall(wrapper, function(err)
	print(err)
	print(debug.traceback())
end)



print('success !!!')