
function pitch(x, y, z)

   x = x + 5
   return x, y, z

end

function yaw(x, y, z)

   y = y + 5
   return x, y, z

end

function roll(x, y, z)

   z = z + 5
   return x, y, z

end
function placement()
	math.randomseed(os.time())
	local x = math.random(1,2);
	local y = math.random(70,85);
	local z = math.random(1,2);
	
	return x, y, z
end 
    


