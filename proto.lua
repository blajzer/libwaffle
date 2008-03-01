samplerate = 8000
freq = 440
position = 0

math.randomseed(os.time())

function sawtooth(num)
	data = math.floor(num * 255)
	if data < 0 then data = 0 end
	if data > 255 then data = 255 end
	return data
end

function sinewave(num)
	data = math.floor(math.sin(num) * 127 + 128)
	if data < 0 then data = 0 end
	if data > 255 then data = 255 end
	return data
end

function noise()
	return math.random(0,255)
end

function square(num)
	data = math.floor(num * 255)
	if data > 128 then data = 255 else data = 0 end
	return data
end

function square25(num)
	data = math.floor(num * 255)
	if data > 192 then data = 255 else data = 0 end
	return data
end

function square75(num)
	data = math.floor(num * 255)
	if data > 64 then data = 255 else data = 0 end
	return data
end


function triangle(num)
	if num < 0.5 then
		return sawtooth(num)
	else
		return sawtooth((1-num))
	end
end

function volume(vol,num)
	return math.floor(num * vol/100) + math.floor(255 * (100-vol)/200)
end

do
local timestep = 40
local time = 0
local value = 100
local trig = true
	function trigger() trig = true end
	function rampdown(num)
		if trig then value = 100 trig = false end
		if value ~= 0 and time % timestep == 0 then value = value - 1 end
		time = time + 1
		return volume(value, num)
	end
	function rampup(num)
		if trig then value = 0 trig = false end
		if value ~= 100 and time % timestep == 0 then value = value + 1 end
		time = time + 1
		return volume(value, num)
	end
end

counter = 0
while true do
	cyclepos = math.fmod(position, 2*math.pi)/(2*math.pi)
	--sawtooth(cyclepos)
	--square25(cyclepos)
	--noise()
	sound = volume(50,rampdown(noise()))--square(cyclepos)))
	--sound = rampdown(triangle(cyclepos))
	io.stdout:write(string.char(sound))
	position = position + 2*math.pi*freq/samplerate
	counter = counter + 1
	if counter % 10000 == 0 then trigger() end
end 