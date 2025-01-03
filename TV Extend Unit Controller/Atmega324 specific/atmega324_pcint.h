#ifndef ATMEGA324_PCINT_H_
#define ATMEGA324_PCINT_H_

void ActivatePinChangeInterrupts()
{
	PCICR = (1 << PCIE3) | (1 << PCIE2); // Any change on any enabled PCINT23..16 and PCINT23..16 pin will cause an interrupt

	// specific pin declaration
	PCMSK3 = (1 << PCINT26) | (1 << PCINT27) | (1 << PCINT28) | (1 << PCINT29) | (1 << PCINT30) | (1 << PCINT31);
	PCMSK2 = (1 << PCINT21) | (1 << PCINT22) | (1 << PCINT23);

	// make sure the global interrupt flag is set
	sei();
}

#endif /* ATMEGA324_PCINT_H_ */