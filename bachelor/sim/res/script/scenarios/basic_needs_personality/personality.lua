personalities = { -- Also traits
    slow_digestion = {
        name = "Slow Digestion", -- Personality trait
        desc = "People with this get hungry 20%% slower", -- A description of what it does (Shows up as tool tip)
        
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