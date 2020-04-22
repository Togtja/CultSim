traits = { -- Also traits
    slow_digestion = {
        name = "Slow Digestion", -- a Trait
        desc = "People with this trait get hungry 20%% slower", -- A description of what it does (Shows up as tool tip)

        can_inherit  = true, -- This can be transfered to its children (default false)
        inherit_chance = 0.7, -- If can_inherit is true, you can set a chance to inherit(probability run twice if both parents has it) (default 100%/1.0)
        mutable = true, -- Can get it even if no parents has it (default false)
        mutate_chance = 0.0001, -- chance of getting it when no parents has it (default SOME LOW ASS NUMBER)

        attain_condition = function(self) -- must return a bool, true if it should be attaied 
            local need_c = cultsim.get_component(self, component.need)
            if need_c == nil then
                log.warn("I(" .. self .. ") am invalid")
                return false
            end
            local the_need = cultsim.get_need(need_c, "Hunger")
            if(the_need == nil) then
                log.warn("I(" .. self .. ") can not find the Hunger Component")
                return false
            end
            if the_need.status > 90 then
                return true
            end
            return false
        end,

        lose_condition = function(self)   -- must return a bool, true if it should be removed
            local need_c = cultsim.get_component(self, component.need)
            if need_c == nil then
                log.warn("I(" .. self .. ") am invalid")
                return false
            end
            local the_need = cultsim.get_need(need_c, "Hunger")
            if(the_need == nil) then
                log.warn("I(" .. self .. ") can not find the Hunger Component")
                return false
            end
            if the_need.status < 60 then
                return true
            end
            return false
        end,

        affect = function(self)
            local need_c = cultsim.get_component(self, component.need)
            if need_c == nil then
                log.warn("I(" .. self .. ") am invalid")
                return
            end
            local the_need = cultsim.get_need(need_c, "Hunger")
            if(the_need == nil) then
                log.warn("I(" .. self .. ") can not find the Hunger Component")
                return
            end
            the_need.decay_multi = the_need.decay_multi - 0.2
        end,
    
        unaffect = function(self)
            local need_c = cultsim.get_component(self, component.need)
            local the_need = cultsim.get_need(need_c, "Hunger")
            the_need.decay_multi = the_need.decay_multi + 0.2
         end
    },
   fast = {
        name = "Fast Runner",
        desc = "People with this trait runs 20%% faster",
        --todo if no attain or lose default to this:
        attain_condition = function(self)
            return false
        end,

        lose_condition = function(self)
            return false
        end,

        affect = function(self)
            local move_c = cultsim.get_component(self, component.movement)
            if move_c == nil then
                log.warn("I(" .. self .. ") am invalid")
                return
            end
            move_c.speed_multi = move_c.speed_multi + 0.2
        end,

        unaffect = function(self)
            local move_c = cultsim.get_component(self, component.movement)
            if move_c == nil then
                log.warn("I(" .. self .. ") am invalid")
                return
            end
            move_c.speed_multi = move_c.speed_multi - 0.2
        end

   }
}