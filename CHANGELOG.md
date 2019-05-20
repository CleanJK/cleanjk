# cleanjoke changelog

## March 2019

- everything in ojk until jan 1 2018
- removed siege gametype
- removed NPCs (including vehicles) and leftover q3 bot/ai navigation code
- default saber is now `single_1`
- `models/weapons2` renamed to `models/weapons`
- default player name is "Player" not "Padawan"

## April 2019

New cvars:

Name | Default | Description
|:--- |:---:| ---:|
cg_instantDuck | 1 | don't smooth camera movement when ducking
cg_gunBob | 0.005 0.01 0.005 | ???
cg_gunBobEnable | 1 | gun will move around when running
cg_gunIdleDrift | 0.01 0.01 0.01 0.001 | ???
cg_gunIdleDriftEnable | 1 | gun will drift around when standing still
cg_gunMomentumDamp | 0.666 | smoothness of moving gun around
cg_gunMomentumFall | 0.5 | ???
cg_gunMomentumEnable | 0 | gun will sway around with camera movement
cg_gunMomentumInterval | 2.5 | ???

- Removed Tavion and Desann saber styles
- Server heartbeat every 2m (was 5m)
- Removed legacy VM support layer
