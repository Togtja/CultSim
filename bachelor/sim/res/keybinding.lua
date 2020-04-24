-- Lua configuration file for the application

action_key_bindings = {
	[EKeyContext.PreferenceScene] = {
		["Escape"] = EAction.EscapeScene,
	},
	[EKeyContext.LoadScenario] = {
		["Escape"] = EAction.EscapeScene,
	},
	[EKeyContext.Agent] = {
		["Escape"] = EAction.EscapeScene,
	},
	[EKeyContext.PauseMenu] = {
		["Escape"] = EAction.EscapeScene,
	},
	[EKeyContext.EditorScene] = {
		["Escape"] = EAction.EscapeScene,
	},
	[EKeyContext.DefaultContext] = {
		["Escape"] = EAction.Quit,
	},
	[EKeyContext.AgentOnHover] = {
	},
	[EKeyContext.ScenarioScene] = {
		["F"] = EAction.FollowEntity,
		["W"] = EAction.MoveUp,
		["D"] = EAction.MoveRight,
		["S"] = EAction.MoveDown,
		["Space"] = EAction.Pause,
		["E"] = EAction.ZoomOut,
		["Q"] = EAction.ZoomIn,
		["."] = EAction.SpeedUp,
		["Down"] = EAction.MoveDown,
		["Right"] = EAction.MoveRight,
		["Escape"] = EAction.PauseMenu,
		["A"] = EAction.MoveLeft,
		["P"] = EAction.PauseMenu,
		[","] = EAction.SpeedDown,
		["Up"] = EAction.MoveUp,
		["Left"] = EAction.MoveLeft,
	},
}
action_mouse_bindings = {
	[EKeyContext.PreferenceScene] = {
	},
	[EKeyContext.Agent] = {
	},
	[EKeyContext.PauseMenu] = {
	},
	[EKeyContext.EditorScene] = {
	},
	[EKeyContext.DefaultContext] = {
	},
	[EKeyContext.AgentOnHover] = {
	},
	[EKeyContext.ScenarioScene] = {
		[EMouse.BtnWheelUp] = EAction.ZoomIn,
		[EMouse.BtnWheelDown] = EAction.ZoomOut,
		[EMouse.BtnLeft] = EAction.SelectEntity,
	},
}