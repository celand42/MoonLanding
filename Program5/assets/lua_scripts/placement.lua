-- Determines new position for falcon
function placement(counter)
	math.randomseed(os.time()+counter)
	local x = math.random(1,2);
	local y = math.random(70,85);
	local z = math.random(-10, -5);
	
	-- Sets the falcon to the left or the right hand side
	if x==1 then
		x = -20
	else
		x = 20
	end
	
	return x, y, z
end 
    
-- Randomly assigns central force of force push
function force(counter)
	math.randomseed(os.time()+counter)
	local x = math.random (-25, 25);
	local y = math.random (-25, 25);
	local z = math.random(-300, -150);

	return x, y, z
end

