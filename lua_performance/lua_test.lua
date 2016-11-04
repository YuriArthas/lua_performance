
local f1,f2,f3,f4,f5,f6,f7,f8


local i = 0
local count = 0

f1 = function()
	f2()
	f2()
	f2()
	f2()
	f2()

	print('count: ', count)
end

f2 = function()
	f3()
	f3()
	f3()
	f3()
	f3()
end
f3 = function()
	f4()
	f4()
	f4()
	f4()
	f4()
end
f4 = function()
	f5()
	f5()
	f5()
	f5()
	f5()
end
f5 = function()
	f6()
	f6()
	f6()
	f6()
	f6()
end
f6 = function()
	f7()
	f7()
	f7()
	f7()
	return f7()
end
f7 = function()
	f8()
	f8()
	f8()
	f8()
	
	return f8()
end

function f8()
	for _ = 1, 1000 do
		i = i + 1
	end
	count = count + 1
end

return f1

