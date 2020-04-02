personalities = { -- Also traits
    slow_digestion = {
        name = "slow digestion", -- Personality trait
        desc = "People with this get hungry at lot slower than other people", -- A description of what it does (Shows up as tool tip)
        
        affect = function(self)
            local need_c = get_component(self, component.need)
            local the_need = get_need(need_c, "Hunger")
            -- TEST REMOVES IT COMPLETLY
            if(the_need == nil) then
                log.warn("I(" .. self .. ") can not find the Hunger Component")
                return
            end
            the_need.weight_multi = 0
        end,
    
        unaffect = function(self)
            local need_c = get_component(self, component.need)
            local the_need = get_need(need_c, "Hunger")
            the_need.weight_multi = 1
         end
    }
   
}