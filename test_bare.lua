local speed = package.loadlib("./build/libfzf.so", "luaopen_libfzf")()

local function lines_from(file)
  local lines = {}
  for line in io.lines(file) do
    lines[#lines + 1] = line
  end
  return lines
end

local function benchmark(fn)
  local total = 0
  -- Warmup
  for _ = 1, 5 do
    total = total + #fn()
  end

  local start = os.clock()
  total = 0
  for _ = 1, 1 do
    total = total + #fn()
  end
  local end_ = os.clock()
  return (end_ - start) * 1000, total
end

local function filter(prompt, lines)
  local results = {}

  local slab = speed.allocate_slab()
  local p = speed.parse_pattern(prompt)
  for _, line in ipairs(lines) do
    local score = speed.get_score(line, p, slab)
    if score > 0 then
      local positions = speed.get_pos(line, p, slab)
      table.insert(results, { line, positions, score })
    end
  end
  speed.free_slab(slab)
  speed.free_pattern(p)
  return results
end

local lines = lines_from "./files"

print "fzf-native c library performance:"
print("Lines: " .. #lines)
print ""

local time, total = benchmark(function()
  return filter("fzf.c", lines)
end)
print "Native:"
print(" -> Total: " .. total)
print(" -> Time:  " .. time .. " ms")
