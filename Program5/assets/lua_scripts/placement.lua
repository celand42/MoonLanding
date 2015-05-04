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

-- Determines if saber swings
function statistics(statistic, x_pos, y_pos)
	math.randomseed(os.time());
	--local testing_hit = math.random(0,1);
	--if testing_hit > statistic then
	--	if y_pos <= 20 and y_pos >= 20 then
	--		if x_pos > 0 then
	--			return "L"
	--		else 
	--			return "R"
	--		end
	--	end
	--else
	--	return "N"
	--end
	
	return "N"

end

