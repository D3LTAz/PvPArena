# PvP Arena — Free Community Assets for the Demo (Maps, Characters, Weapons)

Compiled by studio-director from technical-artist + level-designer research, 2026-09-09. Live-searched (Fab, Sketchfab, itch.io, Kenney, Epic docs) — not from memory, since marketplace availability and licensing change constantly. Treat every link below as "worth checking live," not "confirmed free forever" — a couple of specific leads below already turned out to be expired promos.

**Headline correction to what we assumed going in:** Quixel Megascans is **no longer free via Fab**. It was free to claim in full through Dec 31, 2024 only; since 2025 it's individually priced again ($4.99/scan on the live seller page). If anyone on the team has an old Epic account that claimed the library before that cutoff, their claimed set stays free forever — worth a two-minute check before paying for anything.

---

## Character

**New top lead, found 2026-09-09 — [Fab: "Quantum Modular Character Free Sample"](https://www.fab.com/listings/quantum-modular-character-free-sample) (share link: https://fab.com/s/f0475e248da1).** Live-verified free (Standard License), UE 5.0–5.8, single UE5 skeleton, **MetaHuman-compatible**, fbx + native Unreal format, and tagged directly for our genre: Military / Tactical / Soldier / Shooter / Rifle / Gameready. Bonus: includes weapon static meshes. This is a free sample/lead-magnet for Quantum Assets' paid modular bundles ($99.99–$399.99 for the full 1000+ module library) — the free download is one pre-built modular character, not the whole kit — but that one character is a much closer out-of-the-box match to "grounded tactical soldier" than Manny/Quinn, at the same zero cost. **Recommend technical-artist pull this in and compare rig/texture quality in-engine before defaulting to Manny/Quinn.**

**Fallback recommendation: UE5's built-in Manny/Quinn mannequin, animated with free Mixamo animations retargeted via UE5's built-in IK Retargeter.** Zero cost, zero licensing ambiguity (it's part of the engine), and the retargeting workflow is a standard, well-documented community pattern. Limitation: visually generic out of the box — needs a cheap re-skin/clothing pass to read as "tactical soldier." Still the safe backup if the Quantum sample's rig/texture quality disappoints in-engine.

- Manny/Quinn — ships free with every UE5 project.
- [Mixamo](https://helpx.adobe.com/creative-cloud/faq/mixamo-faq.html) (Adobe, free) — confirmed royalty-free for commercial game use, including tactical locomotion/aim/reload/death animations.

**Alternative — Epic's free Paragon character art**, confirmed still free via Fab's `Price: Free` filter, commercial use explicitly allowed (can't recreate Paragon itself, but original games built on the assets are precedented — *Predecessor*, *Fault*, *Sparrow*). Higher base quality than Manny/Quinn, but the roster is fantasy/sci-fi MOBA heroes — needs a re-costume pass to fit our grounded tactical identity. [Free Epic Games Content](https://dev.epicgames.com/documentation/unreal-engine/free-epic-games-content-for-unreal-engine) · [Paragon assets](https://www.unrealengine.com/paragon)

**Not recommended for the live roster — MetaHuman.** Free to create, but its realistic-human render style clashes with our tone, it's heavier for a multiplayer arena with many networked characters, and the tactical-clothing packs that would make it read as a soldier had their prices hidden behind Fab's checkout (not confirmed free). Fine for a menu/marketing hero shot later, not the in-match character.

---

## Weapon

**New top lead, found 2026-09-09 — [Fab: "FPS Weapon Bundle" by Deadghost Interactive](https://fab.com/s/4e9d5df313a2).** Live-verified free ("Add to My Library"), 7 weapons + 4 attachments, **all rigged for animation**, alternative textures included for every firearm. This is Epic's old "Permanent Free Collection" (originally an Unreal Marketplace sponsored pack from 2018, migrated to Fab), well-reviewed (4.6/5, 377 ratings) — a known-good, widely-used free pack, not an obscure upload. Two things to check before committing to it: (1) listed compatibility is **UE 4.12–4.27**, not explicitly UE5 — almost certainly imports fine (skeletal meshes/rigs carry forward), but confirm in-engine rather than assuming; (2) license terms show as **"UE Marketplace"** rather than Fab's current "Standard License" — same general commercial-use-permitted intent, but contracts-manager should give it a quick look since it's a different terms page than the Standard License assets elsewhere in this doc. Rigged + textured is a real edge over the low-poly pack below — recommend technical-artist try this one first.

**More free placeholder candidates, checked 2026-09-10:** [AR-15 style rifle](https://www.fab.com/listings/9d67f325-8817-4385-9871-02ad21b73a8c) (SpatialNeglect, free, Standard License, realistic/tactical) and [FPS Guns 4K - Rifle 1](https://www.fab.com/listings/a7e70f32-2091-416c-8da9-1546b4dff1bb) (Heart State Games, free, 4K PBR). Neither is rigged (no animation/skeleton included) — same gap as the M4 Modular Kit below. If you need something that works today without a rigging pass, use Deadghost's FPS Weapon Bundle or the ALowPolyCount rifle/SMG instead — those three are the only rigged, ready-to-use free weapons found so far.

**High-fidelity option, found 2026-09-10 — [Fab: "M4 Modular Kit" by Karnaval](https://fab.com/s/6b60e748a4c0).** Live-verified free, Standard License, 30–50k tris depending on configuration with per-part texture sets — real visual detail, closer to a "polished" hero weapon than any other free option on this list. Two things that matter before treating it as a drop-in: (1) the file is literally named `m4noskel.fbx` — **no skeleton, not rigged.** This is a static mesh to be rigged, not a plug-and-play weapon like the Deadghost bundle; it's the better source mesh but doesn't remove the "first-person arms rig" work already flagged as a real gap in RISK/GAPS below. (2) The author's own listing flags known issues: preview rendering is broken in their tool and roughness/metal maps are "off" as shipped — expect a materials cleanup pass in Unreal. Crediting the creator (Karnaval) is requested, though not a hard license requirement. Good pick if the team has rigging time budgeted; not a fast placeholder like the options below.

**Also checked 2026-09-09 — [Fab: "Low Poly Assault Rifle 1"](https://fab.com/s/2b4e6e56a447) and its companion [Fab: "Low Poly SMG 45"](https://fab.com/s/74d7fc92308d), both by ALowPolyCount.** Live-verified free, Standard License, both rigged with removable parts (mag/sight/stock/charging handle) and camo-customizable UVs. Smaller in scope than Deadghost's bundle (single weapons, not seven, and only 2–4 ratings vs. 377) and no native Unreal format — ship as Blender/fbx/obj in a zip, so manual import either way. Worth keeping as backup single-weapon options if the Deadghost bundle's rig doesn't work out, but the Deadghost bundle above is the stronger pick — go there first.

**No free source found this session hits the "polished" bar on its own outright** — every free option is either unconfirmed texture fidelity or visibly stylized/low-poly. Other free starting points, cleanest license first:

- [Sketchfab — Firearms_Kit_1.0](https://sketchfab.com/3d-models/firearms-kit-10-fbx-cc0-c89000ea5b1440ca8baa772b84702fac) — **live-verified 2026-09-09, license correction: this is NOT CC0.** The listing's title/URL slug says "CC0" but the page's actual License field reads **"CC Attribution" (CC-BY)** — commercial use is still fine, but it legally requires a credit line to the author (britdawgmasterfunk) somewhere in the game (credits screen/README is enough). Don't treat it as public-domain-no-strings; contracts-manager should log the attribution requirement wherever we track third-party credits. Also: **1.3M triangles / 669k vertices** as shipped — this is sculpt/scan-density, not a game-ready mesh; budget real decimation/retopo time before it's usable in-engine, not just a straight import. Still a legitimate zero-cost starting point, just not the "no work needed" option it looked like.
- [Fab — "Weapons FREE - Low Poly 3D Models Pack"](https://www.fab.com/listings/9939041c-a505-47f5-a636-ee51381787ea) — **live-verified free 2026-09-09** (Standard License), 8 models (shotgun, sniper, AR, revolver + more), fbx/glb/obj, Unreal Engine format included. Likely below the "polished" fidelity bar; fine as placeholder/secondary. This is now the **lower-effort recommended pick over the Sketchfab kit** for a quick placeholder, precisely because it doesn't need the retopo pass the Sketchfab mesh does.
- [Kenney — Blaster Kit](https://kenney.nl/assets/blaster-kit) (CC0, confirmed free) and [Quaternius — 50 Lowpoly Guns](https://quaternius.itch.io/50-lowpoly-guns) (itch, typically CC0) — stylized low-poly, good for blockout/prototyping, not final art.
- itch.io's [FPS+gun](https://itch.io/game-assets/free/tag-fps/tag-gun) tag pages have more candidates but blocked automated checking — browse directly and **read each pack's individual license line**, itch has no site-wide standard.

The PBR/4K, attachment-detailed tactical rifles that actually match our visual bar (e.g. Fab's "Modern Assault Rifle" and "Tactical Assault Rifle" listings) are currently **paid**, typically in the $20–60 range. Worth checking Fab's rotating free-for-two-weeks program periodically — this exact category shows up there regularly.

**Realistic budget takeaway:** free assets get a serviceable placeholder weapon into the game today at zero cost. A first-person arms rig and a signature/polished weapon silhouette are real work regardless of which mesh we start from — that's the piece worth budgeting contractor time for, not a whole weapon-from-scratch commission.

---

## VFX (muzzle flash, tracers, impacts)

- [Fab — Muzzle Flash (Niagara System)](https://www.fab.com/listings/435b3bcb-d7f5-467d-99aa-2edc97a6c5fd) — **live-verified free 2026-09-09** (Standard License, UE 5.4), Niagara-native, drop-in ready.
- **Epic's free Niagara Examples Pack for UE 5.7** — 50+ ready-to-use systems (explosions, bullet impacts, fire, smoke, lightning, dust) with a gallery level. This is also the best lead for Substation's electrical theme (lightning/arc effects for the "flicker" beats). [80.lv writeup](https://80.lv/articles/epic-releases-over-50-free-niagara-systems-for-unreal-engine-5-7) · [Epic's announcement](https://www.unrealengine.com/news/discover-over-50-free-niagara-systems-ready-to-use-in-unreal-engine-5-7)
- **Correction:** UE5 Starter Content (which used to have basic fire/spark particles) was **removed starting UE 5.6/5.7** and can't be added to new projects on current engine versions. Don't plan around it — confirm which UE5 version we're actually building on.
- Flickering substation lights are free, native Blueprint/Sequencer work (animate a Point Light's intensity) — not an asset to source at all.

---

## Substation map (industrial/electrical kit)

**Time-sensitive find, 2026-09-09 — [Fab: "Industrial Infrastructure" by Sierra Division](https://fab.com/s/80121e18b0e5).** Live-verified: normally $59.99, currently **100% off (free) as a limited-time promo — sale ends September 22, 2026, 6:59 AM.** UE 5.1–5.8, native Unreal format, 5.0★ (8 ratings). Tag list is a near-exact match for Substation's needs: catwalks, pipes, support beams, railings, ladders, vents, rusty/weathered metal panels, wall panels — assets only, no pre-built environment. **Action needed before 09/22/2026:** Add to Library while the promo is live — once claimed it's permanent even after the sale ends, but missing the window means paying $59.99 later. Free price shown is for the Personal license tier (default selection); doesn't matter at our current scale but worth a glance at checkout.

Quixel Megascans is off the table as a free option now (see headline correction) — use these instead:

- Fab's permanent free filter (`fab.com/search?is_free=1`), combined with keywords `industrial`, `warehouse`, `modular environment` — the standing mechanism, best day-to-day source even though individual prices need checking live (Fab's price field is client-rendered, couldn't be verified by automated fetch this session).
- [Epic's free Paragon environment art](https://www.unrealengine.com/paragon) — free and commercial-use confirmed, but fantasy/MOBA-styled — usable for generic industrial geo/props, not a strong grounded-industrial match on its own.
- Chain-link/fence kit leads worth checking: [Modular industrial Fence kit](https://www.fab.com/listings/3c86473d-4e47-4a20-a564-2271643e6500), [Modular Chain Link Fence](https://www.fab.com/listings/d7a97d00-52e0-4346-a7cd-fea65f64fd2a).
- A specific 165-piece modular warehouse pack (ScansMatter) was free through Aug 26, 2025 — almost certainly reverted to paid now, listed here only as a pattern to watch for ("check Fab's free rotation regularly, packs like this go free for two-week windows").

**Free texture alternative to Megascans** (concrete, rust, asphalt, chain-link — exactly what Substation needs): [Poliigon free tier](https://www.poliigon.com/textures/free), [TextureCan](https://www.texturecan.com/tag/Rust/), [3dtextures.me](https://3dtextures.me/) — all genuinely free CC0/free-for-commercial PBR scans, not tied to the Megascans paywall.

## "The Ring" map (barrier/cover props)

**Update 2026-09-09 — gap partially closed.** Prior pass found no confirmed-free barrel/crate/barrier pack; that was a false negative caused by Fab's search results not rendering for automated fetch tools, not an actual lack of supply. Live browser verification found free barrel cover confirmed:

- [Fab — "Weathered Blue Industrial Barrel - PBR 4K Texture" by Orange3D](https://www.fab.com/listings/11d5b3e0-56ec-4a88-8225-2ad2e12f6844) — **live-verified free 2026-09-09** (Standard License), full 4K PBR set (BaseColor/Normal/Metallic/Roughness/Displacement), 2,742 verts (game-ready, no retopo needed), tagged "Industrial." Good fit for both The Ring's cover and Substation's dressing. Fab's `is_free=1` filter combined with a `barrel` keyword search surfaced multiple other free barrel results too — this looks like a well-populated category, not a one-off.

Two of the three specific leads flagged last pass are now confirmed, not just suspected:
- [Fab — "Two concrete jersey barriers road traffic walls"](https://www.fab.com/listings/e58b0a63-3600-49a8-ad96-b4a040ecb018) — **live-verified PAID 2026-09-09**, $8.99–$11.99.
- [Military Barriers Pack](https://www.fab.com/listings/fbbc8866-f498-440b-aaf7-f04b57371602), [Metal And Concrete Barrier](https://www.fab.com/listings/55d3da51-5032-4dbe-874f-e3d21ff579f1) — not yet live-checked; treat price as unconfirmed until someone opens them in a real browser (Fab's price field doesn't render for automated fetch).

**Net for The Ring:** the barrel is a confirmed zero-cost win for cover dressing. Crates and the jersey-barrier-style hard cover are still not confirmed free — either keep hunting Fab's free filter with the same live-browser method, or just buy the jersey barrier pack above ($8.99–$11.99, already price-confirmed) if the map needs that exact silhouette. This remains the cheapest gap in the whole list either way.

---

## RISK / GAPS — what free assets genuinely don't cover

1. **First-person arms rig** — no free weapon source includes one; every sourced mesh needs reposing/rigging regardless of which we pick.
2. **A distinctive, ownable weapon silhouette or the Transformer Pit's specific hero geometry** — free kits are generic by nature; a signature look still needs solo time or a small contractor commission.
3. **MetaHuman-quality character in tactical gear, fully free** — clothing pack prices weren't confirmed free.
4. **Crate/jersey-barrier hard cover for The Ring** — barrels are now a confirmed free win (see above), but crates and barrier-wall silhouettes still aren't confirmed free. Cheapest gap left in the list — just buy it ($8.99–$11.99 for the jersey barrier pack, price-confirmed) if the free hunt doesn't turn one up.
5. **Fab licensing tier tied to revenue**: Standard License splits Personal (under $100K/yr) vs. Professional (over) — doesn't block us at current scale, but re-check every Fab-sourced asset's tier if/when token revenue approaches that line. Worth a standing note in `docs/finance-budget-tracker.md`.
6. **Time-limited "free" drops churn constantly** — anything named in a 2025 article should be re-verified live before counting on it.
7. **"CC0" labels on marketplace listings aren't reliable** — the Sketchfab firearms kit's title says CC0 but its actual License field says CC-BY (attribution required). Confirm the License field itself, not the listing title/slug, before assuming a zero-obligation license.

**Verification note (2026-09-09):** automated WebFetch/WebSearch could not render Fab's client-side price fields or its search-result grids, which produced at least one false negative in the original pass (The Ring's "no confirmed-free barrel pack" — a live browser found several). Everything tagged "live-verified" above was checked with a real rendered browser, not a fetch tool; everything not yet tagged that way should get the same treatment before anyone budgets around its assumed price or license.

**Net:** free assets can credibly cover the skeleton/animation/base-VFX layer and get a placeholder weapon, free barrel cover for The Ring, and most of Substation's generic industrial geometry into the game at zero cost today. They don't replace a contractor for the final "polished" bar on the hero weapon, first-person arms, and The Ring's remaining crate/barrier cover — budget contractor time for those specifically rather than a full from-scratch art contract.
