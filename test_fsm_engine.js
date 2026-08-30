/**
 * ==============================================================================================
 * DSD Micro Project: Automated FSM Logic & Matrix Verification Test Suite
 * Course: Digital System Design (DSD)
 * 
 * Usage: node test_fsm_engine.js
 * ==============================================================================================
 */

function runDeterministicFSM(targetSequence, inputSequence) {
  let currentState = 0; // 0: S0, 1: S1, 2: S2, 3: S3, 4: S4
  const transitions = [];

  for (let i = 0; i < inputSequence.length; i++) {
    const bit = parseInt(inputSequence[i]);
    const fromState = currentState;
    const expected = parseInt(targetSequence[currentState]);

    if (bit === expected) {
      currentState++;
    } else {
      currentState = (bit === parseInt(targetSequence[0])) ? 1 : 0;
    }

    transitions.push({
      step: i + 1,
      bit: bit,
      from: `S${fromState}`,
      to: `S${currentState}`,
      outputZ: (currentState === 4) ? 1 : 0
    });
  }

  return {
    finalState: `S${currentState}`,
    isUnlocked: (currentState === 4),
    transitions: transitions
  };
}

function runFullMatrixTest(target) {
  console.log(`\n=================================================================`);
  console.log(`🔍 RUNNING 16-STATE VALIDATION MATRIX FOR TARGET: [${target}]`);
  console.log(`=================================================================`);
  console.log(` Index | Injected Code | Final State | Expected | Verdict`);
  console.log(`-------+---------------+-------------+----------+---------`);

  let passed = 0;

  for (let i = 0; i < 16; i++) {
    const code = i.toString(2).padStart(4, '0');
    const result = runDeterministicFSM(target, code);
    const expectedUnlock = (code === target);
    const isCorrect = (result.isUnlocked === expectedUnlock);

    if (isCorrect) passed++;

    const idxStr = (i + 1).toString().padStart(5, ' ');
    const codeStr = code.padStart(13, ' ');
    const stateStr = result.finalState.padStart(11, ' ');
    const expStr = (expectedUnlock ? 'S4 (Z=1)' : 'DENIED').padStart(8, ' ');
    const verdictStr = isCorrect ? ' ✓ PASS' : ' ✗ FAIL';

    console.log(`${idxStr} | ${codeStr} | ${stateStr} | ${expStr} | ${verdictStr}`);
  }

  const coverage = ((passed / 16) * 100).toFixed(0);
  console.log(`-----------------------------------------------------------------`);
  console.log(`🎯 Test Result: ${passed}/16 Combinations Passed (${coverage}% Coverage)`);
  
  if (passed === 16) {
    console.log(`✅ FSM Mathematics Verified: ZERO FALSE POSITIVES / FALSE NEGATIVES.\n`);
  } else {
    console.error(`❌ Validation Failure Detected!\n`);
    process.exit(1);
  }
}

// Execute matrix tests for multiple reconfigured targets
console.log(`⚡ DSD SYNCHRONOUS FSM HARDWARE VERIFICATION SUITE`);
runFullMatrixTest("1011"); // Primary Target
runFullMatrixTest("1100"); // Reconfigured Target 1
runFullMatrixTest("0101"); // Reconfigured Target 2
runFullMatrixTest("1111"); // Reconfigured Target 3

console.log(`🎉 ALL RECONFIGURED FSM COMBINATIONS PASSED VERIFICATION WITH 100% INTEGRITY.`);
