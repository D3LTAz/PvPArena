# PvP Arena — Pre-Launch Content Calendar (Build-in-Public Phase)

**Prepared by:** social-media-manager
**Covers:** first ~6-8 weeks of public content, starting before there is a playable build.
**Status:** All copy below is drafted only. I don't post — publishing any of this is Daniel's action. Nothing here names a ship date or confirms an unshipped feature; everything is framed as "in development," because that's the only honest framing until game-director locks something further.

---

## CALENDAR

Dated cadence, roughly 2 posts/week early on (Discord + X only — no assets needed), scaling up to TikTok and YouTube once build-side has footage to hand over. Entries marked **[READY]** are fully drafted below and postable today. Entries marked **[GATED]** need something from build-side first (see NEEDS) and are topic placeholders, not drafted copy — I'll draft them the moment the asset or confirmation lands.

| Date | Week | Channel | Post | Status |
|---|---|---|---|---|
| Sep 7 (today) | 1 | X | Kickoff post — building in public, no date, no hype | **[READY]** — Draft 1 |
| Sep 7 (today) | 1 | Discord | Server welcome / pinned announcement | **[READY]** — Draft 2 |
| Sep 10 | 1 | X | Devlog #1 thread — why server-authoritative netcode came before any content | **[READY]** — Draft 3 |
| Sep 14 | 2 | Discord | "Why tactical, not arcade" — design philosophy post, text only | [GATED — none, can draft next] |
| Sep 17 | 2 | X | Devlog #2 — UE5 + Lyra-as-reference, what we're taking and what we're not | [GATED — none, can draft next] |
| Sep 21 | 3 | Discord | Editor screenshot — first look at the project in UE5 (non-spoiler, tooling-focused) | [GATED — needs UE5 editor screenshot] |
| Sep 24 | 3 | X | Same screenshot, shorter caption, cross-posted | [GATED — needs UE5 editor screenshot] |
| Sep 28 | 4 | TikTok | First process clip — short screen-capture of graybox blockout in progress, no commentary needed | [GATED — needs level-designer screen capture] |
| Oct 1 | 4 | Discord | Devlog #3 — graybox map walkthrough (stills or clip), what a "graybox" is and why we ship gameplay before art | [GATED — needs graybox screenshots/clip] |
| Oct 5 | 5 | YouTube | Devlog video #1 — narrated, engine + architecture focused, editor footage + diagrams, no gameplay required | [GATED — needs UE5 editor capture + narration] |
| Oct 8 | 5 | X | Clip pull from YouTube devlog (60-90s cutdown) | [GATED — depends on YouTube devlog #1] |
| Oct 12 | 6 | Discord | Weapon/character art progress (concept or early model, whatever technical-artist has) | [GATED — needs technical-artist asset] |
| Oct 15 | 6 | TikTok | Second process clip — art pipeline (concept sketch to blockout, or similar) | [GATED — needs technical-artist screen capture] |
| Oct 19 | 7 | Discord | Milestone 1 progress check-in — deathmatch loop status, framed as "in progress," not "coming soon" | [GATED — needs game-director status confirmation] |
| Oct 22 | 7 | X | Same milestone update, condensed | [GATED — needs game-director status confirmation] |
| Oct 26 | 8 | Discord | First gameplay clip — server-authoritative hit reg in action, 1v1 graybox | [GATED — needs QA-cleared gameplay clip; hard gate, see NEEDS] |
| Oct 29 | 8 | YouTube + TikTok | Cutdowns of the first gameplay clip for both platforms | [GATED — same as above] |

Cadence logic: weeks 1-2 run on text and decisions we can already talk about (locked architecture, design philosophy) because none of it needs an asset. From week 3 on, cadence is deliberately paced slower than "2x/week" wherever a slot needs build-side material — I'd rather skip a week than post filler. The first actual gameplay clip (week 8) is the natural point to reassess cadence with studio-director, since after that the content well opens up a lot.

---

## CHANNEL

| Content type | Channel | Why |
|---|---|---|
| Short announcements, decision-explainer threads, screenshot drops | **X** | Fastest read, best for dev-Twitter/X audience that already follows tactical shooter development; threads are the right format for "why we made this call" posts. |
| Longer-form updates, community tone-setting, anything with room to breathe | **Discord** | Owned space, no algorithm, the right place for the fuller version of a post that gets trimmed for X. This is also where people who become playtesters later are already sitting — community-manager owns what happens to them once they're there. |
| Short vertical process footage (blockout timelapses, editor screen-capture, art pipeline clips) | **TikTok** | Only channel worth the format for us. Kept to process/craft footage, not gags — a tactical-shooter audience on TikTok responds to "here's how this gets made," not meme energy. No content scheduled here until there's real footage to show; a TikTok post that's just a screenshot with text-to-speech reads as filler and undercuts the tone. |
| Narrated devlogs, milestone walkthroughs | **YouTube** | Lowest cadence, highest production value. First video can run without gameplay footage (editor capture + narration over the architecture decisions), but the channel earns its keep once there's a clip worth walking through. |

---

## DRAFT

### Draft 1 — X — Sep 7 — [READY]

> Building a tactical multiplayer shooter in Unreal Engine 5 — server-authoritative hit registration and lag compensation from day one, not bolted on later.
>
> No release date. No hype cycle. Just a small team building in public. Follow along.

(238 characters, fits a single post. No thread needed.)

---

### Draft 2 — Discord — Sep 7 — [READY]
*(Pinned in #announcements)*

> **Welcome — here's what this server is.**
>
> This is PvP Arena's build-in-public space. We're a small team making a tactical multiplayer shooter — think in the neighborhood of CS and Valorant, not battle royale, not a hero-shooter, not arcadey. Clean gunfights, server-authoritative hit registration, decisions that reward being right over being lucky.
>
> A few things worth knowing up front:
>
> — **There is no release date, and we're not going to invent one to sound exciting.** When there's a real date, it'll come from the team actually building the game, not from a hype calendar.
> — **This is genuinely pre-playable right now.** We're building the foundation first — netcode, architecture, the stuff that's invisible until it's wrong — so the early posts here are going to be about decisions and process, not screenshots. That changes as the build catches up.
> — **We post progress here first**, usually before it goes anywhere else. If you want the earliest look at anything, this is it.
> — **Feedback and questions are welcome** — we'll flag what we can answer and what's still too early to answer honestly.
>
> More soon. Glad you're here for the unglamorous part.

---

### Draft 3 — X — Sep 10 — [READY]
*(4-tweet thread)*

**1/**
> Devlog #1: why we built server-authoritative netcode before we built a single gun.
>
> In a tactical shooter, hit registration IS the game. If players can't trust what they saw, nothing else matters — not the maps, not the weapons, not the economy.

**2/**
> Before any content went in, we locked the architecture: server-authoritative hit registration with lag compensation, built into the client-server model from line one — not bolted on after the game "feels good" in a single-player build. There is no single-player version of this.

**3/**
> We're building in Unreal Engine 5, referencing Epic's Lyra sample for game mode and replication patterns — not shipping it as-is, using it as a map for how UE5 wants multiplayer structured.
>
> Slower start now. Fewer rewrites later.

**4/**
> No release date yet. No feature list to promise. Just the unglamorous work of getting the foundation right before there's anything worth screenshotting.
>
> More of this as it happens.

(Character counts: 246 / 279 / 231 / 181 — all under the 280 limit.)

---

## NEEDS

Before later calendar entries move from [GATED] to postable, I need the following from build-side, routed through studio-director:

**Assets**
- UE5 editor screenshot(s) — non-spoiler, tooling/setup focused (for the week 3 Discord/X pair).
- Screen-capture of graybox blockout in progress, vertical or croppable to vertical, no commentary needed (for the week 4 TikTok slot).
- Any stills or short clip of the finished graybox map, once level-designer has one to show (week 4 Discord devlog).
- Editor footage suitable for voiceover (architecture/systems shots, not gameplay) for the YouTube devlog #1 — this can happen before a playable build exists.
- Concept art or early weapon/character model from technical-artist, whatever stage is presentable (week 6 Discord + TikTok).
- A short, QA-cleared gameplay clip of the deathmatch loop working (1v1 graybox, hit registration visible) — this is the hard gate for week 8 and the first real "look, it works" moment. Needs QA sign-off before it goes out, not just a raw capture.

**Confirmations**
- game-director sign-off that Milestone 1 status can be described publicly as "in progress" with specifics (week 7 update) — I won't post anything about Milestone 1 progress without this, even framed as "in development."
- Explicit confirmation before I draft anything that touches a date, a shipped feature, or a locked scope item beyond what's already in `/docs/decisions.md`.

**Logistics (not mine to own, flagging so nothing stalls on it)**
- Confirmed handles/URLs for X, Discord invite, TikTok, and YouTube channel — I've written channel-agnostic copy, but someone needs to own the actual account creation and give me the handles so future drafts can include correct @mentions/links.
- Whether community-manager's playtester intake process is far enough along to reference in posts (even a soft "playtesting will open later" line) — until then I'm deliberately not mentioning testing access anywhere in the copy above.
