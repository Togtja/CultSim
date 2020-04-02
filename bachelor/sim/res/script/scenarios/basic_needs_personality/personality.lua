personalities = { -- Also traits
    slow_digestion = {
        name = "slow digestion", -- Personality trait
        desc = "People with this get hungry at lot slower than other people", -- A description of what it does (Shows up as tool tip)
        
        affect = function(self)
            log.warn("Getting componet")
            local need_c = cultsim.get_component(self, component.need)
            if need_c == nil then
                log.warn("I(" .. self .. ") am invalid")
                return
            end
            local the_need = cultsim.get_need(need_c, "Hunger")
            -- TEST REMOVES IT COMPLETLY
            if(the_need == nil) then
                log.warn("I(" .. self .. ") can not find the Hunger Component")
                return
            end
            the_need.decay_multi = the_need.decay_multi - 1.2;
        end,
    
        unaffect = function(self)
            local need_c = cultsim.get_component(self, component.need)
            local the_need = cultsim.get_need(need_c, "Hunger")
            the_need.decay_multi = the_need.decay_multi + 1.2;
         end
    }
   
}