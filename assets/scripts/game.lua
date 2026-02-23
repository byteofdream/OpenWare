-- OpenWare Lua demo callback
-- Called from fixed update: ComputeHeroImpulse(dt, timeSeconds)

function ComputeHeroImpulse(dt, timeSeconds)
    -- Tiny scripted pulse every ~4 seconds for demonstration.
    local cycle = math.fmod(timeSeconds, 4.0)
    if cycle < dt then
        return 2.5
    end
    return 0.0
end
