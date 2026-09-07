# PvP Arena — Token Economy Structure Proposal

**Author:** sales-monetization
**Status:** Draft for legal / platform compliance review — not yet approved for build
**Date:** 2026-09-07

This proposal is written to the locked constraints in `docs/decisions.md` and the sales-monetization agent spec: PvP-only wagering, optional, no real-money cash-out, tokens purchasable with real money in addition to being earned in-game, single-currency design (geofenced/age-gated rather than split into a separate "wager-only" currency), shipping as a finished feature at launch. None of those five constraints are relitigated below — this document is the concrete structure built inside them.

Currency name used throughout: **Arena Tokens (AT)**.

---

## 1. How tokens are earned in-game

Design goal: earning is tied to *playing matches*, not to skill, spend, or grind-to-power. A player who never pays should be able to fully participate in wagering and eventually afford any cosmetic through play alone. Nothing about earn rate should let a paying player out-earn a non-paying player of equal playtime — the loop only affects *speed of accumulation for purchases*, never match outcomes.

**Base earn mechanism (participation-gated, not performance-gated):**

| Event | AT awarded | Notes |
|---|---|---|
| Completed match (any mode: 1v1/2v2/3v3, casual or ranked) | 10 AT | Paid once match reaches a valid completion state server-side (see anti-abuse below). Same for win or loss — deliberately flat so it rewards showing up, not stomping. |
| Match won | +5 AT | Small win bonus on top of the flat 10. Kept small and non-scaling (not tied to KDA, damage, streaks) so it can't be read as "pay/perform-to-win" — it rewards the match outcome, not individual stat-padding. |
| First match of the day | +15 AT (one-time, resets daily) | Standard daily-engagement bonus, common industry pattern, not wager-specific. |

**Daily earn cap:** 150 AT/day from play (roughly 10 completed matches). Purpose: prevents bot/smurf farming of tokens for resale-adjacent behavior and caps the free-to-play accumulation rate the economy has to be priced against (see pricing, §2). The cap is on *earned* AT only — it does not limit purchased AT or wager winnings.

**Anti-abuse gates on earning (needed before this can ship, flagged for engineering, not decided here):**
- A match only pays out if it reaches a minimum duration and both sides had active input (no AFK-farming a bot-filled or conceded match for token income).
- Earn-eligible matches must come from matchmaking or a verified custom lobby with distinct accounts on both sides — same-household/same-IP repeated pairings should throttle or flag (this list doubles as the collusion check wagering needs in §3).
- No AT for spectating, practice/bot matches, or tutorial matches.

**Explicit non-goal:** there is no mechanism anywhere in this proposal by which spending real money increases per-match earn rate, match rewards, or win probability. Purchased AT and earned AT are the same token, same wallet, same spending power — the *only* thing money buys is AT directly (§2), not a better rate of getting it.

---

## 2. Token pricing tiers (real-money purchase)

Reference rate: **100 AT ≈ $1.00** at the smallest tier, with increasing bonus AT at higher tiers (standard bundle-discount shape). This reference rate matters beyond pricing — it's the number legal/compliance will need to assess "real-world value of a token" (see COMPLIANCE §5).

| Tier | Price (USD) | Base AT | Bonus | Total AT | Effective rate |
|---|---|---|---|---|---|
| Starter | $4.99 | 500 | — | 500 AT | 100 AT/$ |
| Small | $9.99 | 1,000 | +100 (10%) | 1,100 AT | 110 AT/$ |
| Medium | $19.99 | 2,000 | +400 (20%) | 2,400 AT | 120 AT/$ |
| Large | $49.99 | 5,000 | +1,500 (30%) | 6,500 AT | 130 AT/$ |
| Max | $99.99 | 10,000 | +4,000 (40%) | 14,000 AT | 140 AT/$ |

Notes:
- Prices/AT counts are a starting proposal, not final — finance-ops should sanity-check against target ARPU once available; the *shape* (flat entry rate, increasing bonus at volume) is the part worth locking early since it's what compliance will review.
- No subscription-based AT drip is proposed at launch — keeps the "real-money value of a token" calculation simple for legal review (one purchase price ladder, not a blended subscription rate).
- Platform cut (Steam ~30%, console equivalents) applies to these prices as normal IAP — not modeled specially here, that's a finance-ops/contracts-manager number.

---

## 3. What tokens can be wagered on, and how a wager resolves

**Eligible matches:** PvP only — 1v1, 2v2, or 3v3 deathmatch, ranked or custom lobby. Never against bots, never in practice/tutorial modes, never in modes without a clean win/loss result. Both locked constraints (PvP-only, optional) are enforced at the match-creation step: wagering is an opt-in toggle on the match/lobby, off by default, and requires every participant to individually opt in — a match is never wagered because one player wants it to be.

**Wager tiers:** 10 / 25 / 50 / 100 / 250 AT per player, flat — both sides (or all sides in team formats, split evenly) must commit the same amount. Proposed cap of 500 AT wagered per match, and a proposed **daily wager-loss cap per account** (e.g., 1,000 AT/day) as a harm-reduction control — see COMPLIANCE §6 on whether this is legally required or just good practice.

**Escrow mechanics, in plain terms:**
1. When all participants confirm the wager amount, the backend economy service **debits each participant's AT balance immediately** into a server-side escrow record tied to that match ID. This happens before the match starts — nobody is holding an IOU, the tokens are already out of both wallets.
2. The match plays out on the authoritative game server as normal (same server-authoritative result reporting already used for ranked outcomes).
3. On match completion, the game server reports the authoritative result to the economy service, which resolves the escrow record:
   - **Win:** winning side's account(s) receive the full escrow pool minus the studio rake (proposed 5–8%, see below). Losing side receives nothing back.
   - **Verified draw** (if the ruleset ever produces one): full refund to all participants, no rake taken.
   - **Disconnect/forfeit inside the existing grace-period window** (the same window ranked already uses for reconnect attempts): escrow refunded in full to all participants — an early network blip doesn't cost anyone their wager.
   - **Disconnect/forfeit outside the grace window:** scored as a loss for the disconnecting party under the same forfeit rule ranked already applies to match outcomes; escrow resolves as a normal win/loss.
4. Escrow state is persisted server-side (not client-held), so a client crash, alt-tab, or app close cannot cause a wager to be lost or duplicated — resolution only ever happens once, keyed to the match ID, off the server's authoritative result.

**Rake (studio revenue on wagers):** proposed 5–8% taken from the pooled wager on a win, credited to studio revenue. This is structurally the one piece of this design that most resembles a classic gambling-operator rake (e.g., a poker room) rather than an IAP transaction, and is flagged as the top scrutiny item in COMPLIANCE §5 below — the percentage itself is illustrative, not decided.

**No cash-out, no secondary market (hard constraint, enforced at the account/wallet level):** AT earned from wagering has no exchange path to real money, cannot be traded, gifted, or transferred between accounts, and cannot be sold. It is spendable in the in-game store only (§4). This is the design lever the whole proposal leans on to argue "not gambling," and it's exactly the lever COMPLIANCE §5 flags as legally unresolved — a no-cash-out policy has not reliably been sufficient on its own in past industry precedent.

**Collusion / abuse controls needed (flagged for engineering, not decided here):** same-account, same-household, or statistically-linked accounts repeatedly wagering against each other and trading losses should be detected and throttled — this reuses the pairing-pattern signal already needed for earn-rate anti-abuse in §1.

---

## 4. What tokens can buy (cosmetics/skins/characters)

Store catalog is **cosmetic-only, zero gameplay effect**, by design constraint — this is what keeps the "buy tokens with real money" path from becoming pay-to-win regardless of how big a player's wallet is:

- Weapon skins (tiered rarity: Standard / Rare / Elite / Legendary — pricing scales with rarity tier, TBD by technical-artist output).
- Character/operator skins and alternate outfits.
- Victory emotes, finishers, kill-cam banners.
- Player calling cards, name colors, profile borders.
- Character/operator **unlocks**, conditional on confirming with game-director that all playable characters are stat-identical and differ cosmetically/thematically only. **This is a dependency, not yet confirmed** — if any character has a distinct ability or stat profile (common in hero-shooter-adjacent design), unlocking that character with tokens would itself be a pay-to-win vector and must be pulled from the token store or made free-unlock-only. Flagging this explicitly so it isn't missed at implementation time.

Hard rule for the store, stated for engineering as an acceptance criterion regardless of who builds it: **no item purchasable with AT (earned or bought) may affect hit registration, damage, movement, weapon stats, matchmaking, or any other gameplay variable.** This rule is what makes §1's "earning isn't pay-to-win" claim and this section's "spending isn't pay-to-win" claim both hold at once.

---

## 5. COMPLIANCE — flagged for legal / platform review before launch

Everything in this section is a **studio-director escalation to Daniel** per the sales-monetization agent spec — this is analysis, not a decision, and none of it should be treated as cleared just because it's written down concretely.

1. **The core open legal question: does "real-money-purchasable tokens, wagered PvP, no cash-out" still count as regulated gambling?** No-cash-out is the load-bearing assumption of this whole design, and it has not reliably insulated products from gambling classification in past industry precedent — regulators and courts have in some cases looked past the absence of an official cash-out to whether the token/item has *any* ascertainable real-world value (here, it plainly does — §2 prices it directly) or whether a secondary market exists (this design blocks trading, which helps, but blocking trading was also true of some products that still drew scrutiny). This needs a gaming/gambling-law specialist's read on our specific jurisdictions before this ships as designed, not after. This is the single biggest risk in the whole proposal.

2. **Geofencing — implementation is undecided and needs a jurisdiction list from legal, not from sales-monetization.** Open items: which IP-geolocation vendor to use, how to handle VPN/proxy evasion, and — critically — *which jurisdictions to exclude*, which is a legal call, not a design one. Likely candidates to ask counsel about explicitly: US states with skill-gaming/sweepstakes statutes that have previously been applied to virtual-item wagering; countries with specific loot-box/wagering regulatory activity (e.g., Belgium, Netherlands); any market requiring a skill-gaming or gambling operator license to run cash-adjacent wagering at all. We should not guess this list ourselves.

3. **Age-gating — self-attestation (a birthdate field) is probably not sufficient for a wagering feature**, even though it's normal for general app age gates. If legal determines this is gambling-adjacent, it likely needs real age/ID verification specifically gating the wagering toggle (separate from account creation), and a minimum age that may be 18 or 21 depending on jurisdiction rather than the lower general app age. Vendor and implementation TBD pending that legal read.

4. **Platform policy exposure — Steam and console storefronts each need a direct compliance check, not an inference from this document.** Valve has specific history and rules around gambling-adjacent mechanics tied to in-game items (the CS:GO skin-gambling situation is the relevant precedent) and may require special disclosure, a different store classification, or refuse the feature outright depending on how it's implemented. Console holders (PlayStation, Xbox, Nintendo) have their own certification requirements around loot-box/wagering-style mechanics, including possible ESRB/PEGI "Simulated Gambling" or interactive-elements descriptor changes that can affect the game's age rating storewide, not just for this feature. Each platform needs its own compliance contact engaged before submission — sales-monetization can prep the technical description of the feature for that conversation but cannot have that conversation or make the account/agreement calls (those are Daniel's per the agent spec).
5. **The wager rake (§3) is the design element structurally closest to a gambling-operator's cut** (e.g., a poker room's rake) rather than an ordinary IAP margin, since it's revenue taken from player-vs-player stakes rather than from a direct sale. Flagging this as the item most likely to draw regulator/platform attention even if the rest of the design clears review — legal should weigh in on whether taking a rake specifically (versus, say, only monetizing via §2 purchases and leaving wagers rake-free) changes the risk picture enough to be worth the revenue tradeoff.
6. **Responsible-gaming controls are proposed but not confirmed as required.** Daily wager-loss caps, spend limits, and self-exclusion options are recommended in §3 as good practice regardless of legal classification, but whether any of them are a legal *requirement* (versus just prudent) depends on how the core question in item 1 resolves.
7. **Payment/tax/AML obligations exist independent of the gambling question.** Real-money token sales (§2) trigger standard payment-processor and sales-tax handling regardless of how the wagering question resolves — this is adjacent, not blocking, but should route to finance-ops/contracts-manager in parallel rather than waiting on the gambling-law answer.
8. **Underage/excluded accounts still need a defined experience.** If age-gating or geofencing excludes an account from wagering, that account should still be able to buy/earn/spend AT on cosmetics (§4) — the exclusion is scoped to the wager feature specifically, not the whole economy. Worth explicit legal confirmation that this scoping is sound rather than assuming it.

---

## Summary (deliverable format)

```
PROPOSAL
Single currency "Arena Tokens" (AT), earned via flat per-match participation
rewards (10 AT/match + 5 AT win bonus + 15 AT daily-first-match, capped at
150 AT/day) and purchasable in five price tiers ($4.99-$99.99, 100-140 AT/$
with volume bonus). Wagerable only in PvP 1v1/2v2/3v3 matches, opt-in per
match, flat tiers of 10-250 AT, server-side escrow debited at match start
and resolved off the authoritative match result, 5-8% studio rake on wins,
no cash-out/no trading ever. AT spends only on cosmetics: weapon/character
skins, emotes, banners, profile cosmetics, and character unlocks pending
confirmation that all characters are stat-identical. No AT purchase or
wager path affects gameplay power at any point.

COMPLIANCE
Biggest open item: no-cash-out may not be sufficient on its own to avoid
gambling classification given tokens have a directly-priced real-money
value (§2) — needs a gaming/gambling-law specialist read before this ships
as designed. Geofence jurisdiction list, age-verification method, and
Steam/console platform sign-off are all undecided and explicitly not ours
to decide. Wager rake is the single element most structurally similar to
a gambling operator's cut and is flagged as the top scrutiny item within
an already-flagged system.

REVENUE MODEL
Two streams: (1) direct AT sales for real money — primary, predictable,
same mechanics as standard IAP; (2) rake taken from PvP wager pools —
secondary, variable, and the legally sensitive stream. Cosmetics are the
token sink that sustains demand for stream (1) beyond wagering. Nothing
in the model sells gameplay power, so there's no pay-to-win revenue path
to defend separately.

RISK
Shipping PvP token wagering at launch without prior legal/platform
sign-off risks storefront rejection, regional bans, or gambling-law
liability — the "no cash-out" design choice is necessary but may not be
sufficient, and that gap is not ours to close without a lawyer.

NEEDS FROM DANIEL
- Retain/consult gaming-gambling-law counsel to answer COMPLIANCE item 1
  before build starts, ideally before this feature is scheduled at all.
- Approve (or send to counsel to produce) the actual geofence exclusion
  list and minimum wagering age once legal responds.
- Platform compliance contact/account at Steam and each console holder to
  clear the feature and confirm age-rating impact before submission.
- Payment processor / IAP account setup for real-money AT purchases.
- Decision on the proposed rake percentage (5-8% illustrative) once
  COMPLIANCE item 5 has a legal answer.
- Confirmation from game-director whether all playable characters are
  cosmetically-differentiated only, before character unlocks are added
  to the token store (§4).
```
