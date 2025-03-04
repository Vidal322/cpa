use std::fs::OpenOptions;
use std::io::{Write, Error, BufWriter};
use std::time::{Duration, Instant};

fn on_mult(m_ar: usize, m_br: usize, file: &mut BufWriter<std::fs::File>) -> Result<(), Error> {
    let mut temp: f64;

    let pha: Vec<f64> = vec![1.0; m_ar * m_ar];
    let mut phb: Vec<f64> = vec![0.0; m_br * m_br];
    let mut phc: Vec<f64> = vec![0.0; m_ar * m_ar];

    for i in 0..m_br {
        for j in 0..m_br {
            phb[i * m_br + j] = (i + 1) as f64;
        }
    }

    let time1: Instant = Instant::now();

    for i in 0..m_ar {
        for j in 0..m_br {
            temp = 0.0;
            for k in 0..m_ar {
                temp += pha[i * m_ar + k] * phb[k * m_br + j];
            }
            phc[i * m_br + j] = temp;
        }
    }

    let duration: Duration = time1.elapsed();
    let seconds = duration.as_secs();
    let milliseconds = duration.subsec_millis();

    writeln!(file, "{},{:.3}", m_ar, seconds as f64 + milliseconds as f64 / 1000.0)?;
    Ok(())
}

fn main() -> Result<(), Error> {
    let filename = "rust_output.csv";

    let file = OpenOptions::new()
        .write(true)
        .create(true)
        .truncate(true) 
        .open(filename)?;


    let mut file = BufWriter::new(file);
    writeln!(file, "Size,Time")?;


    for i in (600..=3000).step_by(400) {
        for _ in 0..5 {
            on_mult(i, i, &mut file)?;
        }
    }

    Ok(())
}
