# Community Structure Plan — PvP Arena

Author: community-manager (via studio-director)
Status: design only — nothing below is live until Daniel executes the NEEDS section
Scope basis: Stage 1 locked scope in `/docs/decisions.md` (1v1/2v2/3v3 deathmatch, single graybox map, one starting weapon, server-authoritative hit registration with basic lag compensation, minimal format-picker matchmaking)

This document is the full playbook for opening PvP Arena to outside players: who we let in first, how the Discord is organized and moderated, and where every piece of feedback ends up. It's written to be stood up as-is once there's a build worth testing — it is not a plan to build a plan.

---

## 1. Playtester Recruitment

### 1.1 Why closed and small first

Stage 1 is one map, one weapon, and a netcode stack (server-authoritative hit reg + lag compensation) that has never been tested outside the dev machine. The first cohort's job is to stress that netcode and that one weapon under real network conditions, not to evaluate whether the game is fun yet. A small, closed, high-signal batch beats a large open one for this pass — fewer players means every bug report gets read, and we can hold a bar on who gets in.

### 1.2 Batch size

**Closed Alpha Batch 1: 15–25 players.**

Reasoning:
- Small enough that qa-engineer and game-director can personally read every report and Discord message during the first two weeks.
- Large enough to fill 1v1/2v2/3v3 lobbies concurrently without coordinating a schedule for every single match.
- Expandable in increments of ~20 for Batch 2+ once the intake/triage pipeline below has proven it can absorb the volume — don't scale the cohort before the pipeline is proven.

### 1.3 Application questions

A short form (Google Form / Discord form-bot / Typeform — Daniel's call, see NEEDS) with these fields:

1. **Discord username** (required — this is the primary comms channel)
2. **Platform** — PC only for Stage 1 (Windows build). Anyone answering console/Mac is auto-rejected for this batch with a note that Stage 1 is PC-only.
3. **Typical connection** — wired / wifi, rough region (NA-East / NA-West / EU / Other). Used to deliberately spread testers across network conditions, since lag compensation is explicitly in scope to stress-test.
4. **Hardware baseline** — CPU/GPU/RAM, one line, self-reported. Not a gate, just triage context if performance bugs come in.
5. **Prior competitive/tactical FPS experience** — free text (e.g. "CS2 Faceit level 6", "casual Valorant"). Used to weight selection toward players who can articulate *why* something feels off, not just *that* it does.
6. **Have you filed a bug report before? Can you follow a reproduction-steps format?** — yes/no + optional example. This is the single highest-value screening question: Stage 1 needs testers who can write "I did X, expected Y, got Z," not just "felt bad."
7. **Availability** — days/hours they can actually play in the batch window.
8. **NDA acknowledgment** — checkbox agreeing not to stream, screenshot, or publicly share build content until Daniel/social-media-manager clears it. (Legal wording is a DANIEL/contracts-manager item, not something community-manager drafts.)

### 1.4 Selection criteria

**In scope for Batch 1:**
- PC (Windows), stable internet, available during the test window.
- Demonstrated ability to describe a problem with specifics (question 6 is the primary filter).
- Spread across at least 2 distinct network regions/conditions — reject redundant "all wired NA-East" applicant pools even if individually qualified, to keep lag-compensation coverage real.
- Willing to sign the NDA / content-hold acknowledgment.

**Out of scope / auto-reject for this pass:**
- Console-only or Mac-only players (no Stage 1 build for them).
- Content creators/streamers whose primary interest is capturing footage rather than testing — Stage 1 is pre-NDA-lift and single-map; premature footage is a risk, not a marketing win. (Streamer/creator seeding is a social-media-manager workstream for a later stage, not this one.)
- Anyone unwilling to use the bug-report format, or who leaves question 6 blank/dismissive.
- Duplicate applications / obvious alt accounts.

Selection is a manual review pass, not automatic — recommend Daniel + one delegate read every application rather than approving on a script, given the batch is small enough (15–25) to do by hand.

---

## 2. Discord Server Structure

### 2.1 Channel structure

```
WELCOME
  #start-here          — rules, NDA reminder, how testing works, links to bug/feedback forms
  #announcements        — read-only (mods+devs post only), build drops, patch notes, downtime

PLAYTESTING
  #build-access         — how to download/update the current build, patch notes pinned
  #find-a-match          — LFG/coordination for 1v1/2v2/3v3, since matchmaking is "minimal format-picker" not full auto-queue
  #voice-lobby-1..3      — voice channels for grouped play (start with 3, add on demand)

FEEDBACK  (see Section 3 for routing detail)
  #bug-reports           — structured bug reports only, using qa-engineer's format (pinned template + bot/pinned message enforcing it)
  #design-feedback        — "this weapon feels bad," balance/feel/fun commentary, routed to game-director
  #general-feedback-chat   — unstructured chatter/reactions that don't fit the two structured channels above; triaged out of here into #bug-reports or #design-feedback by mods, not left to sit

COMMUNITY
  #general                — off-topic tester chat
  #clips-screenshots        — NDA-safe internal sharing only (not for public posting)

STAFF (private, mod/dev only)
  #mod-log                 — moderation actions logged here
  #triage                  — where mods stage feedback before it's routed (see Section 3)
```

Keep it this small for Batch 1. Don't pre-build channels for features that don't exist yet (no ranked-mode channel, no cosmetics channel) — channel sprawl in a 15–25 person server reads as noise and dilutes signal.

### 2.2 Roles

| Role | Who | Permissions |
|---|---|---|
| **Owner** | Daniel | Full control |
| **Dev Team** | Daniel + anyone he delegates (game-director/qa-engineer are conceptual agents, not Discord accounts — a human stand-in posts on their behalf until Daniel says otherwise) | Post in #announcements, read all channels, manage messages |
| **Moderator** | TBD — Daniel assigns; not filled by default | Manage messages/timeouts in tester-facing channels, move messages between feedback channels, enforce NDA |
| **Playtester** | Approved applicants only, granted after selection | Access to PLAYTESTING + FEEDBACK + COMMUNITY categories; no STAFF access |
| **Applicant/@everyone** | Unapproved | #start-here only, until approved |

### 2.3 Moderation rules

1. **NDA enforcement is rule #1.** No build footage, screenshots outside #clips-screenshots, or public posting about the game until content-hold is lifted. First violation: warning + removal of offending content. Second: removal from the test batch.
2. **Bug reports go in #bug-reports in the qa-engineer format only.** Off-format posts get moved to #general-feedback-chat by a mod with a pinned-template link, not deleted — don't punish testers for not knowing the format, redirect them.
3. **Design feedback stays constructive and specific.** "This gun sucks" gets a mod nudge toward "what specifically — TTK, recoil pattern, sound feedback?" Not a removal, a redirect.
4. **No harassment, no toxicity, standard Discord ToS enforcement** — this is a small trusted cohort, but the rule exists from day one regardless of size.
5. **Mods triage #general-feedback-chat and #triage daily**, not weekly — with a 15–25 person batch, feedback volume is low enough that daily triage is achievable and keeps qa-engineer/game-director from missing something time-sensitive (especially a critical bug).
6. **Critical bugs (crash, exploit, hit-reg desync) get pulled out of the normal queue immediately** — a mod pings the routing target (see 3.2) the moment one is spotted, doesn't wait for daily triage. This mirrors qa-engineer's own rule that critical bugs escalate immediately rather than batching.

---

## 3. Feedback Routing Pipeline

### 3.1 Intake

All feedback enters through one of three Discord channels:
- `#bug-reports` — structured, using qa-engineer's format
- `#design-feedback` — balance/feel/fun commentary
- `#general-feedback-chat` — catch-all, triaged out daily by mods into one of the above two

No new bug-report format is being invented here — testers use exactly qa-engineer's structure:

```
PROBLEM          one line
EXPECTED         what should happen
REPRODUCTION     exact steps, every time
CAUSE            if known, else "unknown"
FIX              proposed, if you have one
REGRESSION TEST  how to make sure it doesn't come back
```

Testers won't fill every field (CAUSE/FIX especially) — that's expected and fine; a mod or qa-engineer backfills what they can before it's logged. The template is pinned in #bug-reports and linked from #start-here so testers see it before their first post.

### 3.2 Routing

| Feedback type | Destination | How it travels |
|---|---|---|
| Structured bug report | **qa-engineer** | Mod copies/forwards the #bug-reports post into whatever bug tracker qa-engineer works from (or qa-engineer pulls directly from #bug-reports if no external tracker exists yet — TBD, see NEEDS) |
| Critical bug (crash/exploit/hit-reg desync) | **qa-engineer, immediately, out of band** | Mod pings qa-engineer's human operator directly the moment it's spotted — not queued, not batched, matching qa-engineer's own escalation rule |
| Design/balance/feel feedback | **game-director** | Mod summarizes #design-feedback weekly (or ad hoc for anything Daniel flags as urgent) and forwards to game-director; raw thread stays available for game-director to read directly if they want unfiltered tester voice |
| Off-topic / unstructured | **Triaged, not routed** | Mod moves to the correct channel above before it ages more than a day |

Both qa-engineer and game-director only talk to Daniel through studio-director — this plan doesn't change that; community-manager's job ends at getting the right feedback to the right destination channel/queue, not at delivering it to Daniel directly.

### 3.3 Scope discipline

qa-engineer's spec requires flagging anything tested against scope that has since changed. Community-manager mirrors that: if a tester reports something outside locked Stage 1 scope (e.g. requesting a second map, a second weapon, console support), that's logged as **design feedback to game-director**, not as a bug — it's a scope/roadmap signal, not a defect.

---

## STRUCTURE

Three linked systems: (1) a closed-batch playtester recruitment funnel — application form, manual selection against explicit in/out criteria, batch size capped at 15–25 for the first pass; (2) a Discord server with WELCOME / PLAYTESTING / FEEDBACK / COMMUNITY / STAFF channel categories and a four-tier role structure (Owner / Dev Team / Moderator / Playtester), with NDA enforcement as the top moderation priority; (3) a feedback-routing pipeline that funnels structured bug reports (in qa-engineer's exact PROBLEM/EXPECTED/REPRODUCTION/CAUSE/FIX/REGRESSION TEST format) to qa-engineer, design/balance commentary to game-director, and treats out-of-scope requests as design feedback rather than bugs.

## CRITERIA

In scope for Batch 1: PC/Windows players, stable connection, spread across ≥2 network regions, able to write a specific bug report, willing to accept the NDA/content hold. Out of scope: console/Mac players (no build for them), streamers/creators primarily after footage, anyone unwilling to use the bug format, duplicate/alt applications. Batch size: 15–25, expand in ~20-player increments only after this pipeline proves it can absorb the load.

## ROUTING

Structured bugs → qa-engineer (via #bug-reports, forwarded to qa-engineer's tracker/queue). Critical bugs (crash/exploit/hit-reg desync) → qa-engineer immediately, out of band, no batching. Design/balance/feel feedback and out-of-scope requests → game-director (via #design-feedback, weekly summary + raw thread access). Unstructured chatter → triaged daily by mods into one of the above, never left unsorted. Nothing reaches Daniel directly from this pipeline — everything continues through studio-director per both agents' specs.

## NEEDS (DANIEL items — none of this is live until these happen)

- **DANIEL: Create the Discord server** and build the channel/category structure in Section 2.1.
- **DANIEL: Assign the Moderator role** to at least one trusted person (or explicitly hold it himself) — no default moderator exists in this plan.
- **DANIEL: Decide who posts as "Dev Team"** for game-director/qa-engineer, since those are agent roles, not Discord accounts — a human needs to be the hands on the keyboard for #announcements and forwarding bug reports.
- **DANIEL: Stand up the application form** (Google Form, Discord bot, or Typeform) using the questions in Section 1.3, and choose where responses land for review.
- **DANIEL: Decide/confirm qa-engineer's bug tracker destination** — this plan assumes bug reports get forwarded somewhere qa-engineer actually works from; if that's just #bug-reports itself, say so explicitly so no hand-off step goes missing.
- **DANIEL: Approve/reject Batch 1 applicants** (or delegate that review) — selection in Section 1.4 is a manual read, not an automated filter.
- **DANIEL: Finalize and legally confirm NDA/content-hold wording** — Section 1.3's NDA checkbox and Section 2.3's enforcement rule both need actual legal language, which is a contracts-manager/Daniel item, not a community-manager one.
- **DANIEL: Set the Batch 1 test window dates** so the application's "availability" question and build-access timing have something concrete to reference.
