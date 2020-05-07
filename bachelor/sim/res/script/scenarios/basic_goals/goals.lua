--This file keeps track of all possible goals. Not all agents may necessarily use all goals.

goals = {
	eat = {
    name = "eat",
    age = 0,
    tags = ETag.Food,
    weight_function = function(self)
        return self.age*self.age
    end
	,
    change_over_time = function(self,dt)
        return ((self.age+dt)*(self.age+dt)) - (self.age*self.age)
    end
	,
    get_discontentment = function(value)
        return value * value
    end
    }
}






