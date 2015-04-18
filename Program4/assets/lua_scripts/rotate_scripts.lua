function placement()
	math.randomseed(os.time())
	local x = math.random(1,2);
	local y = math.random(70,85);
	local z = math.random(-10, -5);
	
	-- Sets the falcon to the left or the right hand side
	if x==1 then
		x = -10
	else
		x = 10
	end
	
	return x, y, z
end 
    


