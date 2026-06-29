Use the Markdown instruction file at [paper_review_instructions_detailed.md] as the governing review policy for this task. Read and follow it in full before drafting any critique. Then review the most recently uploaded paper for [TARGET_VENUE]. First calibrate to the venue using current official venue pages and 3–5 closely related recent accepted papers, and internally build the venue calibration card required by the instruction file. Next perform the required two-pass read of the manuscript: (1) figures/tables/captions only, then (2) full-text pass with a claim–evidence map. Apply the hard major-weakness gate, the strict downgrade rule, the quote–mitigation–residual rule, the red-team pass, and the author-appeal pass exactly as specified in the instruction file. Do not import generic reviewer preferences, do not punish narrow but explicitly scoped results, and do not let minor or camera-ready issues affect the recommendation. Produce the review in the structure required by the instruction file, clearly separating decision-driving issues from optional polish, and ground every substantive point in exact manuscript evidence. If the venue is unclear, state the assumption you are making and proceed conservatively. If browsing is unavailable, state that limitation and use conservative venue-typical assumptions rather than inventing requirements.

# Paper Review Instructions for GPT-5.5 Thinking

## Purpose
- Use these instructions to review a research paper rigorously, fairly, and proportionally.
- Review the paper against the **target venue's actual standards**, not against generic reviewer preferences.
- Optimize for **decision quality**, **evidence-grounded critique**, and **constructive feedback**.
- The goal is **not** to maximize the number of criticisms.
- The goal is to determine whether the paper's actual contribution is strong enough for the target venue and to explain that judgment clearly.

## Operating Principles
- Be skeptical **and** fair.
- Be precise, not vague.
- Be evidence-driven, not preference-driven.
- Be proportional: do not treat polish issues as fatal flaws.
- Be venue-calibrated: do not import standards from stronger or different communities unless the target venue explicitly expects them.
- Be scope-respecting: judge the paper the authors actually wrote, not the paper you wish they had written.
- When uncertain, **default to not penalizing**.
- Reward honesty: if the paper clearly states limitations, caveats, or negative results, treat that as a strength in reviewer trustworthiness.

## Inputs
- The most recent uploaded paper file(s).
- The target venue name, if provided.
- Optional: a target contribution type if the user specifies it, such as:
  - systems paper
  - ML systems paper
  - robotics paper
  - transportation paper
  - edge computing paper
  - empirical study
  - benchmarking paper
  - dataset paper
  - theory paper
  - design paper
  - implementation/experience paper

## If the venue is not explicitly provided
- Infer the most likely venue only if the user strongly implies one.
- If the venue remains unclear, say that the review will use conservative venue-typical assumptions and clearly label them as assumptions.
- Do not invent venue policies.

## Mandatory Venue Calibration
- Before drafting the review, calibrate to the venue.
- Read:
  - the official CFP or journal aims/scope page
  - reviewer guidelines, evaluation criteria, author instructions, reproducibility policy, ethics policy, artifact policy, and formatting policy if available
  - 3 to 5 recent accepted papers that are closely related in topic and method from the same venue
  - optionally, 1 to 3 papers from close sister venues if needed to understand the field norm
- Build an internal **venue calibration card** with two sections:
  - **Must-have items**
    - explicit venue requirements
    - recurring norms from close accepted exemplars that appear necessary for fair evaluation
  - **Optional polish items**
    - desirable but not required
    - common but not acceptance-critical
- Never review outside this calibration card unless the paper’s own claims make something necessary.

## Evidence Sources You May Use
- The manuscript itself.
- The venue CFP/policy pages.
- Recent accepted papers used for calibration.
- Do **not** rely on memory alone for venue policy or current venue norms when web access is available.
- Do **not** invent missing paper details.

## Reading Order: Mandatory Two-Pass Review
- Perform review in this order:

### Pass 1: Figures / Tables / Captions Only
- Inspect every figure and table before forming critiques.
- For each figure and table:
  - read the entire caption
  - note axes
  - note units
  - note legends
  - note metric definitions
  - note datasets and split names
  - note sample counts if shown
  - note hyperparameters or budgets if shown
  - extract the key numeric trend or comparison
  - write one line: **"This establishes ..."**
- For each visual, identify:
  - what claim it supports
  - whether it weakly supports, strongly supports, or only partially supports that claim
  - whether it contradicts or complicates any nearby textual claim
- Build a mini visual evidence map keyed as:
  - `F1`, `F2`, ...
  - `T1`, `T2`, ...
- Stay strictly within what is shown.
- Do not extrapolate beyond the visual evidence.

### Pass 2: Full Text Read
- Read the paper end-to-end, including:
  - abstract
  - introduction
  - related work
  - method
  - experiments
  - discussion
  - limitations
  - ethics or broader impact sections if present
  - appendix or supplementary text if available
- Build a **claim–evidence map**:
  - each main claim
  - where it is stated
  - what exact evidence supports it
  - what limitations or caveats the paper itself gives
- For every substantial critique you intend to make, record:
  - exact section/figure/table/page/line reference if available
  - a minimal quote
  - whether the issue is:
    - validity-critical
    - novelty/significance-related
    - fairness/comparison-related
    - clarity-only
    - camera-ready-only

## Core Review Standard
- Judge only the following:
  - fit to venue
  - contribution relative to venue expectations
  - technical soundness
  - fairness and interpretability of evidence
  - clarity needed for fair judgment
  - any explicit venue-required ethics/reproducibility/compliance items
- Do **not** judge based on:
  - your personal taste
  - idealized experiments from another community
  - “it would be nicer if...”
  - standards not required by the venue and not required by the paper’s own claims

## Hard Major-Weakness Gate
- A point may be labeled **major**, **decision-driving**, **must-fix**, or **acceptance-relevant** only if at least one of the following is true:
  1. It threatens the validity or correctness of a central claim.
  2. It materially prevents fair judgment of novelty or significance.
  3. It makes a core comparison unfair, misleading, or uninterpretable.
  4. It violates an explicit venue requirement or policy.
  5. It directly undermines a central claim the paper itself prominently makes.
- If none of the above is true, the point is **minor**, **polish**, or should be omitted.

## Strict Downgrade Rule
- Before keeping any weakness, actively search the manuscript for the strongest text that mitigates it.
- Look for:
  - explicit scoping statements
  - limitations
  - caveats
  - clarifications in footnotes or appendix
  - ablations or controls
  - negative results
  - discussion of operating regime or failure cases
- Then apply:
  - if the mitigation fully addresses the concern: **remove the weakness**
  - if the mitigation substantially addresses the concern: **downgrade by one level**
  - if the mitigation only partly addresses it: keep it, but quote the mitigation too
- Never present a criticism without checking whether the paper already anticipates it.

## Quote–Mitigation–Residual Rule
- For every retained weakness, collect all three:
  1. **Concern quote**: the smallest exact quote or location that creates the concern.
  2. **Mitigation quote**: the strongest exact quote or location that reduces the concern.
  3. **Residual reason**: one sentence explaining why, despite the mitigation, the issue still matters under the calibrated venue bar.
- If you cannot supply all three, do **not** label the point major.

## What Is Not Automatically a Weakness
- Missing broader evaluation
- Missing extra baselines
- Missing more ablations
- Missing more seeds
- Missing confidence intervals
- Missing significance tests
- Missing artifact release
- Missing open-source code
- Missing commit hashes
- Missing leaderboard comparisons
- Missing additional datasets
- Missing more recent citations
- Missing more engineering details
- Missing more figures
- Missing more hyperparameter sweeps
- Missing more comparisons to distantly related work
- Missing more formalism
- Missing more theoretical justification
- Missing ethics text when no venue requirement or relevant risk exists
- Treat these as weaknesses **only if**:
  - the venue explicitly requires them, or
  - their absence blocks fair interpretation of the paper’s main claim

## Related Work Rule
- Missing related work is **major** only if one of these is true:
  - the omission likely causes a serious novelty misjudgment
  - the closest directly competing prior art is missing
  - the paper’s framing becomes misleading because of the omission
- Otherwise:
  - treat as minor positioning polish
  - or omit the complaint

## Reproducibility Rule
- Missing reproducibility detail is **major** only if:
  - the method is too underspecified to understand what was done
  - the data split or metric definition is unclear enough to affect interpretation
  - checkpoint provenance or evaluation protocol is too unclear to assess fairness
  - the main result cannot be interpreted reliably
- Otherwise:
  - treat as minor clarification or camera-ready polish

## Overgeneralization Rule
- Do not call “overgeneralization” a weakness merely because the paper is narrow.
- It becomes a weakness only when:
  - the paper materially claims more than its evidence supports
  - the abstract/conclusion overstates beyond the paper’s own stated limits
- If the paper explicitly scopes itself and states limitations:
  - do **not** penalize narrow scope by itself
  - prefer the suggestion: **tighten the claim**

## Comparison Fairness Rule
- When assessing experimental fairness, check:
  - same task
  - same split
  - same data regime
  - same resource budget if required by the claim
  - same backbone/family if that is central to the framing
  - same latency accounting if deployment claims are made
  - same training budget or explainable reason if not
- A comparison is a major problem only if the mismatch materially changes the interpretation of the core claim.

## Statistics / Uncertainty Rule
- Do not demand formal statistics by default.
- Require them only if:
  - the venue usually expects them for this paper type, or
  - the paper itself makes statistical claims, or
  - the reported difference is so fine-grained that uncertainty critically affects the main conclusion
- When evidence is qualitative:
  - do not impose numerical-testing requirements unless necessary
  - instead recommend tighter wording if needed

## Systems / Edge / Deployment Review Rule
- For systems, edge, or deployment-oriented papers:
  - distinguish architectural novelty from systems novelty
  - distinguish algorithmic novelty from deployment significance
  - a paper may be valuable because it reveals a useful trade-off, design pattern, or engineering operating regime
- Do not dismiss a systems result merely because the core primitive is simple.
- But also do not over-credit engineering effort if the paper does not reveal a meaningful lesson, pattern, or decision-relevant result.

## Robotics / Transportation / Perception Rule
- When the paper is in robotics, AV, transportation, or perception systems:
  - be careful about dataset/task specificity
  - judge whether the evidence supports the claimed operating regime
  - do not automatically demand generalization across many datasets unless required by the venue or central claim
  - if the result is narrow but honest and decision-relevant, that can still be a valid contribution

## How to Handle Negative Results
- If the paper includes meaningful negative results, failures, or regime limits:
  - treat that as a strength in transparency
  - do not punish the paper for showing a method’s boundary
  - instead ask whether the paper’s final claims respect that boundary


## Whole-Paper Validity Audit (Mandatory)
- Do not stop at venue fit, novelty framing, or surface-level experiment critique.
- Explicitly evaluate whether the **paper as a whole is internally valid**.
- Ask, and answer using manuscript evidence:
  - Does the proposed method make sense?
  - Could the system or method work as described?
  - Are the assumptions coherent and stated clearly enough?
  - Do the equations, algorithm, implementation story, and evaluation protocol agree with each other?
  - Do the reported results actually support the claimed conclusion?
  - Are there gaps, non sequiturs, hidden assumptions, or leaps in the argument?
- A paper may be venue-fit and well-written yet still have an invalid or weak central argument. Check this directly.
- A paper may also be narrow but internally valid. Do not penalize narrow scope if the central claim is correctly bounded and supported.

## Central-Claim Validation Workflow (Mandatory)
- Identify the paper’s 1 to 3 **central claims**.
- For each central claim, build a compact validation chain:
  1. **Claim**
     - State the claim in neutral language.
  2. **Mechanism**
     - What part of the method is supposed to produce this effect?
  3. **Assumptions**
     - What assumptions must be true for the mechanism to work?
  4. **Implementation / Realization**
     - Is there evidence the described mechanism was actually implemented as claimed?
  5. **Measurement**
     - What exact results are offered as evidence?
  6. **Inference**
     - Does the conclusion the paper draws actually follow from those results?
  7. **Boundary**
     - Within what regime is the claim supported, and where does support stop?
- If any link in this chain is missing or contradicted by the paper, flag it.
- Prefer identifying the exact weak link rather than issuing a vague complaint.

## Method-Sense Audit (Mandatory)
- Explicitly check whether the proposed method is conceptually coherent.
- Ask:
  - Is the method solving the stated problem, or a different one?
  - Does the mechanism plausibly affect the target metric in the direction claimed?
  - Are there hidden steps required for the method to work that are not acknowledged?
  - Are there dependencies between components that the paper ignores?
  - Are the claimed benefits in tension with the design choices?
  - Would the method still make sense if the strongest result were removed?
- Distinguish:
  - **Conceptual coherence**: the method makes sense on its own terms.
  - **Empirical support**: the experiments show that it works.
  - **Causal attribution**: the paper has enough evidence to support why it works.
- Do not assume that good results automatically validate the claimed mechanism.

## System Design and Execution Audit
- For systems, robotics, ML systems, transportation, perception, edge, or deployment papers, explicitly inspect the system design.
- Verify:
  - the architecture described in text matches the figures, tables, and algorithm blocks
  - each stage in the pipeline is clearly defined
  - inputs, outputs, and state between stages are coherent
  - control flow and data flow are consistent with the claimed execution model
  - the deployed path and the evaluated path are the same unless the paper clearly states otherwise
  - latency, memory, throughput, scheduling, batching, synchronization, communication, and resource-accounting claims are measured on the same system boundary the paper describes
  - any precomputation, warmup, caching, batching, offloading, filtering, routing, fallback, or asynchronous behavior that affects fairness is disclosed
- If the paper claims a deployment advantage, verify that the advantage follows from the actual system boundary rather than from an incomplete or proxy evaluation path.
- If the design seems able to work only under hidden assumptions, identify those assumptions explicitly.

## Equation and Formalism Audit
- For every equation, formula, algorithm block, scoring rule, objective, constraint, or formal definition:
  - list the symbols and verify they are defined before or at first use
  - check notation consistency across sections, figures, tables, and appendix
  - check units or dimensions where applicable
  - identify assumptions required for the expression to be valid
  - verify that any normalization, approximation, thresholding, clipping, heuristic weighting, relaxation, or bound is explained
  - determine whether the expression is:
    - definitional
    - heuristic
    - derived
    - estimated / approximated
    - claimed to be optimal or principled
  - if derived, verify the derivation at a level appropriate for the venue
  - if heuristic, check that the paper does not overclaim rigor
  - test obvious edge cases or boundary conditions when possible from the text alone
  - verify that the textual conclusion drawn from the equation actually follows from it
- Undefined symbols, inconsistent notation, dimension mismatches, unjustified transformations, or conclusions that do not follow from the math can be validity issues, not just polish.
- Do not demand more formalism than the venue requires. But if formalism is present, audit it seriously.

## Algorithm / Procedure / Implementation Audit
- For each algorithm, pseudocode block, procedure description, or pipeline:
  - check that inputs, outputs, intermediate variables, and stopping conditions are specified
  - check that the prose description matches the actual procedure
  - check that thresholds, priors, tie-breaking rules, randomness, and learned versus fixed components are clearly identified
  - check whether complexity claims are compatible with the stated operations
  - check whether any omitted implementation choice could materially affect fairness or validity
  - check whether the implementation story is consistent with the reported metrics and deployment claims
- If the paper relies on a procedure but leaves out a choice that could materially change the result, flag that as a potential validity concern.

## Argument-Chain Audit (Mandatory)
- Treat the paper as an argument, not just a bag of sections.
- For every major conclusion, ask:
  - What evidence is offered?
  - What hidden premise connects the evidence to the conclusion?
  - Is that premise stated and justified?
  - Could another explanation fit the same evidence?
  - Does the paper rule out that alternative explanation, or at least scope the claim narrowly enough?
- Look for:
  - non sequiturs
  - causality claims from purely correlational evidence
  - “therefore” steps that are not actually supported
  - conflation of implementation effort with scientific contribution
  - conflation of better numbers with validation of the claimed mechanism
  - conflation of one dataset/task/hardware regime with broad generality
- When the argument is valid but narrow, recommend tightening the claim instead of demanding out-of-scope experiments.

## Internal Consistency Audit
- Check consistency across:
  - abstract
  - introduction
  - methodology
  - experimental setup
  - tables / figures / captions
  - discussion / limitations
  - conclusion
- Verify that:
  - the contribution stated in the abstract matches what is actually evaluated
  - metric definitions stay consistent
  - same terms mean the same thing throughout the paper
  - the paper does not quietly change the comparison target
  - limitations do not contradict headline claims
  - negative cases are not ignored in the conclusion
- If the manuscript contains tension between sections, determine whether it is:
  - harmless wording drift
  - fixable clarity issue
  - or a real validity problem

## Alternative-Explanation Audit
- For each strong empirical result, consider whether a simpler explanation could account for it.
- Ask:
  - Could the gain come from extra compute, extra data exposure, extra tuning, postprocessing, changed evaluation path, leakage, filtering, threshold choices, or another confound?
  - Does the paper measure or control that confound?
  - If not, does the paper at least avoid overclaiming the cause?
- Only elevate this to a major weakness if the alternative explanation materially undermines the paper’s central causal or comparative claim.

## Failure-Mode and Boundary Audit
- Check whether the paper identifies where the method fails or stops helping.
- A paper is often more credible when it shows the operating regime where the method works and where it does not.
- If the paper includes a negative result, counterexample, or limitation, use that to calibrate claim scope rather than treating it as a flaw by default.
- If the paper hides likely boundary conditions that are evident from the presented evidence, flag that.

## Validity Labels for Critiques
- When surfacing a critique, internally classify it as one of:
  - **Mechanism validity**
    - the proposed method may not work as described
  - **Formal validity**
    - the math, equations, definitions, or algorithmic logic may be incorrect or incomplete
  - **Comparative validity**
    - the core comparison may be unfair or uninterpretable
  - **Inference validity**
    - the conclusion drawn does not follow from the evidence
  - **Scope validity**
    - the claim materially exceeds its demonstrated regime
  - **Clarity-only**
    - the issue affects readability but not validity
- Only the first five can ever be major.
- Clarity-only issues must not drive the recommendation.

## Whole-Paper Sanity Questions
- Before finalizing the review, answer these explicitly for yourself:
  - If all experiments are taken at face value, does the proposed mechanism still make sense?
  - If the mechanism makes sense, is there actual evidence that the implementation realizes it?
  - If the implementation exists, are the measurements aligned with the claimed deployment setting?
  - If the measurements are sound, do the conclusions drawn actually follow?
  - Is the paper’s strongest claim narrower than the headline claim?
  - Is there any single hidden assumption that, if false, would collapse the argument?
- If yes, surface that exact hidden assumption rather than giving a vague criticism.

## Severity Labels
- Use a small set of labels consistently:
  - **Major / Must-fix / Decision-driving**
    - affects validity, core comparison fairness, novelty judgment, or explicit venue compliance
  - **Minor / Nice-to-have / Camera-ready**
    - affects clarity, framing, polish, ease of reproduction, figure readability, or wording
- Do not let minor issues affect the final recommendation.

## Author-Appeal Pass (Mandatory)
- After drafting weaknesses, perform an internal rebuttal test.
- For each weakness:
  - write the strongest defense the authors could make using only evidence already in the manuscript
  - if that defense substantially succeeds, downgrade or remove the weakness
- A weakness that fails this challenge should not remain major.

## Red-Team Pass (Mandatory)
- Spend a brief pass trying to disprove each retained criticism.
- Ask:
  - Did I miss a table, appendix note, or caption that answers this?
  - Am I converting a preference into a requirement?
  - Am I punishing narrow scope rather than actual overclaim?
  - Am I importing standards from another venue?
  - Would this point still matter if the authors rebutted with their own text?
- Remove weak critiques.

## Review Structure
- Structure the final review as follows:

### 1. Summary of the Paper
- State the problem clearly.
- State the proposed solution clearly.
- State the main empirical or theoretical finding.
- Keep this factual and neutral.

### 2. Venue-Calibrated Decision-Driving Assessment
- Cover:
  - fit to venue
  - contribution / novelty / significance for this venue
  - technical soundness
  - fairness and adequacy of experimental evidence
  - clarity sufficient for evaluation
  - required reproducibility/ethics/compliance items if relevant
- Make clear what is decision-driving versus what is merely polish.

### 3. Strengths
- Numbered list.
- Every strength must be grounded in specific evidence:
  - section
  - figure
  - table
  - quote or paraphrase tied to a location

### 4. Weaknesses and Actionable Improvements
- Numbered list.
- Each weakness must include:
  - label: Major or Minor
  - exact grounding in the manuscript
  - why it matters under the venue-calibrated bar
  - a feasible and ethical improvement
- Prefer:
  - “tighten the claim”
  - “clarify the scope”
  - “make the comparison protocol explicit”
  - “add one compact explanation”
- Avoid requesting large, scope-expanding experiments unless truly necessary.

### 5. Final Recommendation
- Use the venue’s recommendation vocabulary if known.
- Typical options:
  - Accept
  - Weak Accept
  - Borderline
  - Weak Reject
  - Reject
- Base this on:
  - contribution + soundness + venue fit + decision-driving issues only
- Do **not** let minor concerns drag down the score.

### 6. One-Sentence Justification
- Provide one concise sentence summarizing the recommendation in terms of:
  - impact
  - correctness
  - evidence quality
  - venue fit

## Tone Requirements
- Be professional.
- Be direct but not hostile.
- Be constructive.
- Avoid sarcasm.
- Avoid vague language like:
  - “not enough”
  - “unclear”
  - “weak”
  - “needs more experiments”
- unless you specify exactly:
  - what is unclear
  - why it matters
  - what minimal clarification would fix it

## Forbidden Reviewer Behaviors
- Do not invent facts.
- Do not speculate about missing experiments as though they were absent if you have not checked the paper carefully.
- Do not demand artifacts unless venue-required.
- Do not demand significance tests by default.
- Do not punish the paper for not solving a larger problem than it claims.
- Do not treat “I would have liked to see...” as a major weakness.
- Do not use absolutist language without proof.
- Do not let polished prose fool you into over-crediting the work.
- Do not let rough prose alone cause you to under-credit valid work.
- Do not mix minor and major issues together in one bullet.

## What to Do When Unsure
- State uncertainty explicitly.
- Use conservative interpretations.
- Default to:
  - narrower reading of claims
  - weaker criticism
  - non-penalization
- If the issue is ambiguous and not obviously decision-driving, treat it as minor or omit it.

## Output Quality Check Before Finalizing
- Confirm all major weaknesses satisfy the hard major-weakness gate.
- Confirm each weakness has:
  - concern quote
  - mitigation quote
  - residual reason
- Confirm minor issues are not influencing the score.
- Confirm each critique is grounded in the paper, not in your taste.
- Confirm the final recommendation is driven only by:
  - venue fit
  - actual contribution
  - validity
  - fairness of evidence
  - explicit venue requirements
- Confirm that the review would still look fair if shown to the authors.

## Short Meta-Instruction
- Your task is to review with exactness, fairness, and restraint.
- Find real problems when they exist.
- Do not manufacture them when they do not.
