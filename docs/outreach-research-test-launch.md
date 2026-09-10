# How other indie devs get a demo out for testing + outreach — research notes

Compiled by studio-director, 2026-09-08, to scope the new `web-developer` and `discord-agent` roles.

## 1. Where the build actually lives

**itch.io is the standard low-friction demo host** — free, no approval gate (unlike Steam), and built for exactly this. The CLI tool `butler` is the standard push mechanism:
- `butler push <directory-or-zip> user/game:channel` — channel names use kebab-case and platform keywords (`win`/`windows`, `linux`, `mac`/`osx`) auto-tag the build's platform.
- Re-pushes are fast: butler does block-level diffing (rsync-style) + Brotli compression, so an update to an already-pushed build only transfers what changed.
- Version tags via `--userversion`, and games can query `https://api.itch.io/wharf/latest` to check for updates.
[Pushing builds · The butler manual](https://itch.io/docs/butler/pushing.html)

**Closed testing before public release** — itch.io supports gating a page/build to specific people before it's public: whitelisted user lists, password-protected pages, or secret URLs, independent of the eventual public listing. This is the mechanism for "closed testers only" before the public demo goes live.
[Limited Releases & Playtests - itch.io](https://itch.io/docs/creators/limited-releases)

**Steam Playtest** is the equivalent mechanism on Steam (invite-only test builds through Steamworks) — a separate, heavier setup (Steam Partner account, ~$100 fee) worth treating as its own later decision, not bundled into the itch.io-based demo launch.
[Steam Playtest (Steamworks Documentation)](https://partner.steamgames.com/doc//features/playtest)

## 2. Where testers gather and give feedback: Discord

Discord's own developer guide and independent closed-beta playbooks converge on the same structure:
- **Channel categories:** read-only info / two-way discussion / admin-only / playtest voice rooms.
- **Core channels:** #rules-and-info, #announcements, #patch-notes, #game-discussion, #build-feedback, #report-issue (as a **Forum Channel**, not scrolling text — "a Forum Channel for bugs is dramatically better than scrolling text where reports get buried"), #admin-chat/#admin-log.
- **Minimal roles:** @admin, @playtest, @everyone — with @everyone's default channel visibility revoked, so a leaked invite link doesn't expose the server.
- **Security:** email/phone verification level, Community feature + Rules Screening, media content filters.
- **Bug report template** (per post in the Forum Channel): build version, reproduction steps, expected behavior, platform/hardware, media — this maps directly onto our qa-engineer's existing PROBLEM/EXPECTED/REPRODUCTION/CAUSE/FIX/REGRESSION TEST format, so feedback should route there rather than inventing a second scheme.
- **Bots:** start with off-the-shelf moderation bots (Dyno, ProBot, Ticket Tool) rather than building custom.
[The Game Developer Playbook, Part One: Getting Started on Discord](https://discord.com/blog/the-game-developer-playbook-part-one-getting-started-on-discord)
[How to Run a Closed Beta Test Through Discord: The Complete Playbook | BetaHub](https://betahub.io/resources/how-to-run-closed-beta-test-discord/)

**Recruiting testers**, per the same sources: aim for 50–200 per wave, quality over quantity, source from your own community first, then genre-relevant spaces (e.g. r/playmygame, genre Discords, Steam Next Fest). Use an application form to screen for hardware/experience/timezone diversity. Recognize Discord testers are self-selected, more skilled/forgiving than average players — worth supplementing with outside recruits before trusting difficulty/tutorial feedback.
**Wrap-up:** announce the closing date ~2 weeks ahead, run a final survey, thank contributors publicly (in-game reward is common), archive the feedback for the next phase.

## 3. Press / streamer outreach

The standard indie pattern is a **press kit page** — a single page with the pitch, screenshots, trailer, build/key-request contact — commonly built with a purpose-made tool (e.g. presskit.gg) rather than hand-rolled from scratch, plus a built press list and direct outreach to relevant press/streamers/YouTubers, sometimes via key-distribution services.
[The Complete Guide to Indie Game Press Kits](https://presskit.gg/blog/indie-game-press-kit-guide)
[How to Get Press Coverage for Your Indie Game](https://presskit.gg/field-guides/how-to-get-press-coverage-indie-game)

## 4. What this means for PvP Arena's two new roles

- **web-developer** owns: the itch.io page + `butler` pipeline (build lands here first, access-gated to closed testers, opened to public when the demo's ready), and a press kit page. Steam Playtest is flagged as separate, later scope — not built by default.
- **discord-agent** owns: the technical build-out of the server community-manager already designed (`docs/community-structure-plan.md`) — channels, roles, permissions, off-the-shelf bots — with feedback routed into qa-engineer's existing bug format rather than a new one.

Neither role needs custom software built to start — itch.io + butler, off-the-shelf Discord bots, and a templated press kit page cover the test-phase launch. Custom tooling (a feedback-routing bot, a bespoke site beyond the itch.io page) is real additional scope, priced separately if wanted.
