use std::fs::{File, OpenOptions};
use std::io::{Write, Error};


fn on_mult(m_ar: usize, m_br: usize, filename: &str) {
    
    let mut temp: f64;
    

    let pha: Vec<f64> = vec![1.0; m_ar * m_ar];
    let mut phb: Vec<f64> = vec![0.0; m_br * m_br];
    let mut phc: Vec<f64> = vec![0.0; m_ar * m_ar];

    for i in 0..m_br {
        for j in 0..m_br {
            phb[i * m_br + j] = (i + 1) as f64;
        }
    }

    let time1: std::time::Instant = std::time::Instant::now();
    
    for i in 0..m_ar {
        for j in 0..m_br {
            temp = 0.0;
            for k in 0..m_ar {
                temp += pha[i * m_ar + k] * phb[k * m_br + i];
            }
            phc[i * m_br + j] = temp; 
        }
    }
    
    let duration: std::time::Duration = time1.elapsed();
    append_execution_time(filename, duration.as_secs_f64()).expect("Could not write to file");
}

fn append_execution_time(filename: &str, time_in_seconds: f64) -> Result<(), Error> {
    let mut file = OpenOptions::new()
        .append(true)
        .open(filename)?;

    writeln!(file, "{:.5}", time_in_seconds)?;
    Ok(())
}

fn main() {

    let filename = "rust_output.csv";
    let _file = File::create(filename);
/*
    for i in (600..3000).step_by(400) {

        for _ in 0..5 {
            on_mult(i, i, filename);
        }
    }*/

    on_mult(600, 600, filename);
//    on_multline(5, 5);

//    on_multblock(5, 5, 2);
}











/*    println!("Time: {:.3}", duration.as_secs_f64());

    println!("");
    for i in 0..2 {
        for j in 0..std::cmp::min(10, m_br) {
            
            print!("{} ", phc[i * m_br + j]);
        }
        println!();
    }*/

/*
fn on_multline(m_ar: usize, m_br: usize) {
    

    let pha: Vec<f64> = vec![1.0; m_ar * m_ar];
    let mut phb: Vec<f64> = vec![0.0; m_br * m_br];
    let mut phc: Vec<f64> = vec![0.0; m_ar * m_ar];

    for i in 0..m_br {
        for j in 0..m_br {
            phb[i * m_br + j] = (i + 1) as f64;
        }
    }

    for i in 0..m_ar {
        for j in 0..m_br {
            for k in 0..m_ar {
				phc[i*m_ar + k] += pha[i*m_ar + j] * phb[j*m_br+k];
            }
        }
    }


    for i in 0..2 {
        for j in 0..std::cmp::min(10, m_br) {
            
            print!("{} ", phc[i * m_br + j]);
        }
        println!();
    }
    
}

fn on_multblock(m_ar: usize, m_br: usize, block_size: usize) {
    

    let pha: Vec<f64> = vec![1.0; m_ar * m_ar];
    let mut phb: Vec<f64> = vec![0.0; m_br * m_br];
    let mut phc: Vec<f64> = vec![0.0; m_ar * m_ar];

    for i in 0..m_br {
        for j in 0..m_br {
            phb[i * m_br + j] = (i + 1) as f64;
        }
    }

    for i in (0..m_ar).step_by(block_size) {
        for j in (0..m_br).step_by(block_size) {
            for k in (0..m_ar).step_by(block_size) {
                for ii in i..std::cmp::min(i + block_size, m_ar) {
                    for jj in j..std::cmp::min(j + block_size, m_br) {
                        for kk in k..std::cmp::min(k + block_size, m_ar) {
                            phc[ii * m_ar + kk] += pha[ii * m_ar + jj] * phb[jj * m_br + kk];
                        }
                    }
                }
            }
        }
    }



    for i in 0..2 {
        for j in 0..std::cmp::min(10, m_br) {
            
            print!("{} ", phc[i * m_br + j]);
        }
        println!();
    }
    
}
*/
