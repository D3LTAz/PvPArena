# PvP Arena — Budget Tracker

Owner: finance-ops (tracks and estimates only — **never spends**). All actual
spend, invoice payment, or account/subscription signup is a DANIEL item,
every time, no exceptions. This doc exists so Daniel can log real expenses
starting today without restructuring anything.

Status: Stage 1 structure. No real numbers filed yet — every dollar figure
below is a placeholder or a clearly-marked example.

---

## 1. Budget categories

Standard indie-dev cost buckets for a solo-founder, UE5 multiplayer FPS
project. Use these as the `CATEGORY` values in the tracking table below.
Add a subcategory in the description field rather than inventing new
top-level categories, so totals stay comparable over time.

| Category | Typical indie-dev line items (fill in what applies) |
|---|---|
| **Engine / Tools / Licenses** | UE5 seat/royalty terms, IDE licenses, source control hosting (Git LFS storage/bandwidth), DCC tools (Blender is free; Maya/3ds Max/Substance if used), middleware (Wwise/FMOD, netcode/backend SDKs), asset store purchases, plugin licenses, VPN/dev VMs |
| **Contractor Labor** | Concept art, 3D art/animation, sound design/music, additional programming, level design help, QA/playtesting contractors, translation/localization |
| **Platform / Storefront Fees** | Steam direct fee ($100/app, refundable after threshold), console dev kit/registration fees (if applicable), payment processor cuts, distribution platform revenue share, server hosting for dedicated servers or matchmaking, CDN/bandwidth for patches |
| **Marketing** | Trailer production, key art, ad spend (social/Steam), influencer/creator outreach budget, press kit tools, community giveaways, convention/event fees, Discord boosts/bots |
| **Legal** | Business entity formation/maintenance, ToS/EULA/privacy policy drafting, contractor agreement templates or review, trademark search/filing, platform agreement review |
| **Misc / Contingency** | Domain/hosting for website, email/productivity subscriptions, hardware/peripherals for testing, unexpected small purchases that don't fit above |

Recommendation: hold a contingency line (a percentage of total estimated
Stage 1 budget, set by Daniel) inside Misc, so scope-change estimates below
have somewhere to land before they force a re-budget conversation.

---

## 2. Running-total tracking table

Log every real expense here, oldest first. **Running Total** is the
cumulative sum of all Amount entries above and including that row. The
three rows below are EXAMPLES ONLY — marked as such — to show the format.
Delete or replace them with Daniel's real first entries; do not build on
top of them as if they were real spend.

| Date | Category | Description | Amount | Running Total |
|---|---|---|---|---|
| _EXAMPLE — not real_ | 2026-01-15 | Engine / Tools / Licenses | Git LFS storage plan, monthly | $5.00 | $5.00 |
| _EXAMPLE — not real_ | 2026-01-20 | Contractor Labor | Concept artist, key weapon silhouettes (10 hrs) | $400.00 | $405.00 |
| _EXAMPLE — not real_ | 2026-02-01 | Platform / Storefront Fees | Steam Direct app fee | $100.00 | $505.00 |
| | | | | |

Rules for using this table:
- One row per transaction, not per invoice line item if the invoice has several — break those into separate rows so category totals stay accurate.
- Amount is always what was actually paid, in USD unless Daniel says otherwise (note currency in Description if not USD).
- Running Total only advances on real rows. Never let an estimate or a pending quote get a Running Total entry — estimates live in Section 3, not here.
- If a row needs to be voided (refund, duplicate entry), add a new row with a negative Amount and a Description noting what it corrects — never delete or edit a past row's Amount silently.

---

## 3. Stage 1 cost-estimate structure

Before Stage 1 spend categories have real numbers, each planned or
possible cost gets an estimate line here so Daniel can see the shape of
the budget before committing anything. Fill in the Category rows from
Section 1; leave Estimate blank or as a range until Daniel or a vendor
quote gives a real number.

| Category | What's likely needed (Stage 1) | Estimate (fill in) | Basis for estimate |
|---|---|---|---|
| Engine / Tools / Licenses | UE5 project setup, Git LFS plan, any paid plugins | _TBD_ | _quote / market rate / Daniel's input_ |
| Contractor Labor | Any Stage 1 art/audio/level-design help beyond Daniel + agents | _TBD_ | _quote / market rate / Daniel's input_ |
| Platform / Storefront Fees | Steam Direct fee if wishlist page goes up in Stage 1 | _TBD_ | _known flat fee — confirm current amount_ |
| Marketing | Early trailer/key art if planned this stage | _TBD_ | _quote / market rate / Daniel's input_ |
| Legal | Entity setup, EULA/ToS if not already in place | _TBD_ | _quote / market rate / Daniel's input_ |
| Misc / Contingency | Contingency reserve | _TBD (e.g., % of subtotal)_ | _Daniel's risk tolerance_ |
| **Stage 1 total (estimate)** | | **_TBD_** | sum of above |

---

## 4. Methodology — pricing scope changes in dollars

When game-director or studio-director flags a scope change (a feature
added, a map added, a system reworked), finance-ops prices the dollar
impact using this method:

1. **Identify what changed.** Get the scope description and the
   days/effort estimate from game-director or studio-director — finance-ops
   doesn't re-derive scope, it prices what's already been scoped in time.
2. **Sort the cost drivers into categories.** A scope change usually hits
   more than one category — e.g., a new map might mean Contractor Labor
   (level art help) + Engine/Tools (a new plugin) + Misc (extra Git LFS
   storage for new assets). Price each driver separately, then sum.
3. **For labor-driven cost:** if the added scope requires contractor time
   Daniel wasn't already going to pay for, estimate hours × Daniel's
   assumed or quoted contractor rate for that skill (get the rate from
   Daniel or a real quote — never invent a market rate as fact; if no rate
   is known yet, say so and give a placeholder range instead of a number).
4. **For tool/license-driven cost:** use the vendor's actual published
   price. If none is known, mark the line "price unconfirmed" rather than
   guessing.
5. **For platform/fee-driven cost:** use the platform's published fee
   schedule (flat fees, revenue share %) where it applies.
6. **State assumptions alongside the number**, every time — an estimate
   without its assumptions isn't usable by Daniel or studio-director to
   sanity-check it.
7. **Compare against the category's current total and any contingency
   reserve.** If the new cost would push a category over what's already
   been spent/committed plus contingency, that's a RISK flag, not a
   silent addition.
8. **Report back using the deliverable format below** — this is the
   standard shape for every cost-estimate request finance-ops answers,
   not just scope changes.

### Deliverable format (for all future cost-estimate requests)

```
REQUEST       what's being costed
ESTIMATE      dollars, with the assumptions behind the number
CATEGORY      where it sits in the budget
RISK          one line — if this blows a category or needs Daniel's approval
```

Example shape (illustrative only, not a real estimate):

```
REQUEST       Add a second graybox map to Stage 1 milestone
ESTIMATE      $0–$X — no contractor rate confirmed yet; assumes Daniel/level-designer
              build it in-house with no paid asset purchases. Flag for real
              quote if outside contractor help is brought in.
CATEGORY      Contractor Labor (contingent) / Misc (LFS storage growth)
RISK          Low as scoped (in-house). Escalate to RISK: needs approval if
              contractor help is added — would be new spend outside current budget.
```

---

## 5. Notes

- This file is the single source of truth for PvP Arena spend tracking.
  Don't start a parallel spreadsheet — extend the categories here if a new
  bucket is genuinely needed, via studio-director.
- finance-ops estimates and tracks; it does not execute purchases, sign up
  for subscriptions, or pay invoices. Every real transaction in Section 2
  is something Daniel actually did.
- Update Section 2 the same day a real expense happens, so Running Total
  stays trustworthy as a burn-rate signal.
