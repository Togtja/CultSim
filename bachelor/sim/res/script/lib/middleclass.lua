local a={_VERSION='middleclass v4.1.1',_DESCRIPTION='Object Orientation for Lua',_URL='https://github.com/kikito/middleclass',_LICENSE=[[
      MIT LICENSE
  
      Copyright (c) 2011 Enrique García Cota
  
      Permission is hereby granted, free of charge, to any person obtaining a
      copy of this software and associated documentation files (the
      "Software"), to deal in the Software without restriction, including
      without limitation the rights to use, copy, modify, merge, publish,
      distribute, sublicense, and/or sell copies of the Software, and to
      permit persons to whom the Software is furnished to do so, subject to
      the following conditions:
  
      The above copyright notice and this permission notice shall be included
      in all copies or substantial portions of the Software.
  
      THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
      OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
      MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
      IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
      CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
      TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
      SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
    ]]}local function b(c,d)if d==nil then return c.__instanceDict else return function(self,e)local f=c.__instanceDict[e]if f~=nil then return f elseif type(d)=="function"then return d(self,e)else return d[e]end end end end;local function g(c,e,d)d=e=="__index"and b(c,d)or d;c.__instanceDict[e]=d;for h in pairs(c.subclasses)do if rawget(h.__declaredMethods,e)==nil then g(h,e,d)end end end;local function i(c,e,d)c.__declaredMethods[e]=d;if d==nil and c.super then d=c.super.__instanceDict[e]end;g(c,e,d)end;local function j(self)return"class "..self.name end;local function k(self,...)return self:new(...)end;local function l(e,m)local n={}n.__index=n;local c={name=e,super=m,static={},__instanceDict=n,__declaredMethods={},subclasses=setmetatable({},{__mode='k'})}if m then setmetatable(c.static,{__index=function(o,p)local q=rawget(n,p)if q==nil then return m.static[p]end;return q end})else setmetatable(c.static,{__index=function(o,p)return rawget(n,p)end})end;setmetatable(c,{__index=c.static,__tostring=j,__call=k,__newindex=i})return c end;local function r(c,s)assert(type(s)=='table',"mixin must be a table")for e,t in pairs(s)do if e~="included"and e~="static"then c[e]=t end end;for e,t in pairs(s.static or{})do c.static[e]=t end;if type(s.included)=="function"then s:included(c)end;return c end;local u={__tostring=function(self)return"instance of "..tostring(self.class)end,initialize=function(self,...)end,isInstanceOf=function(self,c)return type(c)=='table'and type(self)=='table'and(self.class==c or type(self.class)=='table'and type(self.class.isSubclassOf)=='function'and self.class:isSubclassOf(c))end,static={allocate=function(self)assert(type(self)=='table',"Make sure that you are using 'Class:allocate' instead of 'Class.allocate'")return setmetatable({class=self},self.__instanceDict)end,new=function(self,...)assert(type(self)=='table',"Make sure that you are using 'Class:new' instead of 'Class.new'")local v=self:allocate()v:initialize(...)return v end,subclass=function(self,e)assert(type(self)=='table',"Make sure that you are using 'Class:subclass' instead of 'Class.subclass'")assert(type(e)=="string","You must provide a name(string) for your class")local h=l(e,self)for w,d in pairs(self.__instanceDict)do g(h,w,d)end;h.initialize=function(v,...)return self.initialize(v,...)end;self.subclasses[h]=true;self:subclassed(h)return h end,subclassed=function(self,x)end,isSubclassOf=function(self,x)return type(x)=='table'and type(self.super)=='table'and(self.super==x or self.super:isSubclassOf(x))end,include=function(self,...)assert(type(self)=='table',"Make sure you that you are using 'Class:include' instead of 'Class.include'")for o,s in ipairs({...})do r(self,s)end;return self end}}function a.class(e,m)assert(type(e)=='string',"A name (string) is needed for the new class")return m and m:subclass(e)or r(l(e),u)end;setmetatable(a,{__call=function(o,...)return a.class(...)end})return a